#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
    bool old = false;
    
    int option = 3;

    Func in("in"), a("a"), b("b"), c("c"), f("f"), g("g"), out("out");
    Var x("x"), y("y");
    ImageParam inp(type_of<int>(), 1, "inp");


    if(old){
        // Input
        a(x) = 4*x + 1; 
        b(x) = 100 - 2*x - 1;
    
        // Output
        c(x) = a(x) + b(x);
        c.ensures(c(x) == a(x) + b(x));

        // Output for options 3 & 4
        out(x) = c(x-1) + c(x) + c(x+1);
        out.ensures(out(x) == c(x-1) + c(x) + c(x+1));
    
        Var bx("bx"), tx("tx");
    
        if(option == 1){
            c.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf); 
        } else if(option == 2){
    
            c.split(x, bx, tx, 32*2, TailStrategy::GuardWithIf);
            c.unroll(tx, 2);
            c.gpu(bx, tx);
        } else if(option == 3){
            out.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
    
            c.compute_root();
            c.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
        } else if(option ==4) {
            out.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
            c.compute_at(out,bx)
                .split(x, bx, tx, 32, TailStrategy::GuardWithIf)
                .gpu_threads(tx);
    
            // Other scheduling options: vectorize, tiling, unrolling
            // Other algorithm options: reductions (summing vallues)
        }
        
    
    
    
        a.compute_root();
        b.compute_root(); 
        if(option == 1 || option == 2){
            c.bound(x,0,128);
            c.output_buffer().dim(0).set_min(0);
        } else if(option == 3 || option == 4){
            out.bound(x,1,128);
            out.output_buffer().dim(0).set_min(0);  
        }
    } else {
        RDom r(0, 10);

        // Func in0;
        // in0(x,y) = 2*x*y;
        // // in(x) = 2 * x;
        // in(x) = 0;
        // in.ensures(in(x) % 2 == 0);
        // // in(x) = in(x) + r;
        // in(x) += in0(x,r);
        // in.ensures(in(x) % 2 == 0);

        // in(x) = 2 * x;
        // in.ensures(in(x) % 2 == 0);

        // Vector<Annotation> outer_annotations;

        // outer_annotations.push_back(requires(forall(x, x>=0 && x<6 ,inp(x) % 2 == 0)));
        // outer_annotations.push_back(ensures(forall({x,y}, x>=0 && x<4 && y>=0 && y<3,out(x,y) % 2 == 0)));


        inp.requires(inp(_) % 2 == 0);
        // inp.requires(forall(x, x>=0 && x<6 ,inp(x) % 2 == 0));

        f(x,y) = inp(x) + inp(y);
        // f.ensures(f(x,y) % 2 == 0);
        f.ensures(f(x,y) == inp(x) + inp(y));
        
        f(x,0) = f(x,0) + f(x,1);
        f.ensures(f(x,y) % 2 == 0);

        g(x,y) = f(x,y)+ f(x+1,y) + f(x+2,y);
        g.ensures(g(x,y) % 2 == 0);

        // in.compute_root();
        g.compute_root(); 
        // f.compute_at(g,y);
        f.compute_at(g,Var::outermost());
        Var xi("xi"), xo("xo");
        g.split(x, xo, xi, 2, TailStrategy::GuardWithIf);
        g.unroll(xi);

        out(x,y) = g(x,y) + 2;
        out.ensures(out(x,y) % 2 == 0);

        // out.ensures(forall({x,y}, x>=0 && x<4 && y>=0 && y<3,out(x,y) % 2 == 0));
        out.compute_root();
        // in.compute_at(g,y);
        // in.compute_at(g,Var::outermost());
        // in.compute_at(g,x);
        

        //out.bound(x,0,4);
        //out.bound(y,0,3);
        out.output_buffer().dim(0).set_bounds(0, 4);
        out.output_buffer().dim(1).set_bounds(0, 3);
        // out.output_buffer().dim(0).set_stride(1);
        out.output_buffer().dim(1).set_stride(4);

        inp.dim(0).set_bounds(0,6);
    }
    

    bool add_runtime = false;
    bool aot = false;
    bool pvl = false;
    bool cplusplus = false;
    std::string s_runntime ("--runtime");
    std::string s_aot ("--aot");
    std::string s_pvl ("--pvl");
    std::string s_cplusplus ("--c++");

    if(argc > 1){
        for(int i=1; i<argc; i++){
            if(s_runntime.compare(argv[i]) == 0){
                add_runtime = true;
            } else if(s_aot.compare(argv[i]) == 0){
                aot = true;
            } else if(s_pvl.compare(argv[i]) == 0){
                pvl = true;
            } else if(s_cplusplus.compare(argv[i]) == 0){
                cplusplus = true;
            } else {
                std::cout << "Error, unknown argument " << argv[i];
                return 1;
            }
        }
    }

    Target target = get_host_target();
    Target new_target = target
      .with_feature(Target::OpenCL)
      .with_feature(Target::NoAsserts)
      .with_feature(Target::NoBoundsQuery)
      ;
    assert(host_supports_target_device(new_target));

    Func res;
    if(option == 1 || option == 2)
        res = c;
    else if(option == 3 || option == 4)
        res = out;
    
    std::vector<Argument> inputs;
    if(old)
        inputs = {};
    else{
        inputs = {inp};
    }

    if(add_runtime){
        compile_standalone_runtime("opencl_runtime.o",new_target);
        res.compile_to_header("even_c.h", inputs, "even", new_target);
    }

    if(cplusplus){
        res.compile_to_c("even_c.cpp", inputs, "even", new_target);
    }

    if(pvl){
        res.compile_to_pvl("even_pvl.pvl", inputs, "even", new_target, outer_annotations);
    }

    if(aot){
        res.compile_to_static_library("even_static", inputs, "even", new_target);
        res.print_loop_nest();
    }

// Func sum(Func input, bool GPU) {
//  RDom r(0, 10);

//  sum() = 0;
//  // 0 + in(0) + in(1) + ... + in(10)
//  sum() = sum() + in(r);
// }
 

//  // This works for CPU's
//  if(!GPU){
//     in.atomic();
//     in.parallel(r)
//  } else {
//     // For GPUs, this is not yet supported
//     in.gpu(r);
//  }
// }

//  Func blur_x, blur_y;
//  Var x, y, xi, yi;

//  // The algorithm - no storage or order
//  blur_x(x, y) = (input(x-1, y) + input(x, y) + input(x+1, y))/3;
//  blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/3;

//  // The schedule - defines order, locality; implies storage
//  blur_y.tile(x, y, xi, yi, 256, 32)
//        .vectorize(xi, 8).parallel(y);
//  blur_x.compute_at(blur_y, x).vectorize(x, 8);

//  return blur_y;
// }


// void box_filter_3x3(const Image &in, Image &blury) {​
//   __m128i one_third = _mm_set1_epi16(21846);​
//   #pragma omp parallel for​
//   ​for (int yTile = 0; yTile < in.height(); yTile += 32) {​
//   __m128i a, b, c, sum, avg;​
//   __m128i blurx[(256/8)*(32+2)]; // allocate tile blurx array​
//   for (int xTile = 0; xTile < in.width(); xTile += 256) {​
//     __m128i *blurxPtr = blurx;​
//     for (int y = -1; y < 32+1; y++) {​
//       const uint16_t *inPtr = &(in[yTile+y][xTile]);​
//       for (int x = 0; x < 256; x += 8) {          ​
//         a = _mm_loadu_si128((__m128i*)(inPtr-1));​
//         b = _mm_loadu_si128((__m128i*)(inPtr+1));​
//         c = _mm_load_si128((__m128i*)(inPtr));​
//         sum = _mm_add_epi16(_mm_add_epi16(a, b), c);​
//         avg = _mm_mulhi_epi16(sum, one_third);​
//         _mm_store_si128(blurxPtr++, avg);​
//         inPtr += 8; }}​
//   blurxPtr = blurx;​
//   for (int y = 0; y < 32; y++) {
//     __m128i *outPtr = (__m128i *)(&(blury[yTile+y][xTile]));​
//     for (int x = 0; x < 256; x += 8) {
//       a = _mm_load_si128(blurxPtr+(2*256)/8);​
//       b = _mm_load_si128(blurxPtr+256/8);​
//       c = _mm_load_si128(blurxPtr++);​
//       sum = _mm_add_epi16(_mm_add_epi16(a, b), c);​
//       avg = _mm_mulhi_epi16(sum, one_third);​
//       _mm_store_si128(outPtr++, avg); }}}}}​

    // f(x,y) = 2*(x+y)+1;
    // f.ensures(f(x,y) % 2 == 1);
    // f(x, x+1) = f(x,x) * 3 + f(x, 1);
    // f.ensures(f(x,y) % 2 == 0);

    // g(x) = f(x, x+1);

    // k(x,y) = 0;
    // k.ensures(k(x,y) == 0);
    // k(x, r2.x - r2.y) = k(x, r2.x) + k(x,0) + r2.y;

    // h(x) = k(x,x);
    // h.ensures(h(x) == 0);
    // h(x) += g(x*8 + r -4) + 1 ;
    // h.loop_invariant(h(x) % 2 == r % 2);
    // h.ensures(h(x) % 2 == 0);

    // in(x) = f(x,x) + h(x+1);//2*x*x+1;
    // //in.context_perm(in(x), frac(1,1));
    // in.ensures(in(x) % 2 == 1);
    // in(x) = select(x < 100, in(x) + 1, 0);
    // in.ensures(in(x) % 2 == 0); 
    
    // //forall*, exists
    // // input_requirement = forall x . x>=0 && x < 100 : input(x) > 0;
    // // output_requirement = forall x . x>=0 && x < 100 : out(x) % 2 == 0 && out(x) > 0;

    // // even.context_perm(in(x), frac(1,2));
    // // even.context_perm(in(x+1), frac(1,2));
    // // even.requires(in(x) % 2 == 0);
    // // even.requires(in(x+1) % 2 == 0);
    // even(x) = in(x) + in(x+1);
    // //even.context_perm(even(x),frac(1,1));
    // even.ensures(even(x) % 2 == 0);

    // Var bx("bx"), tx("tx"), ix("ix"), xy("xy");

    // if(true){
    //     even.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
    //     even.bound(x,0,127);
    //     even.output_buffer().dim(0).set_min(0);

    //     in.in(even)
    //       .compute_at(even, bx)
    //       .gpu_threads(x);

    //     in.compute_root(); //Do not inline
    //     in.split(x, bx, tx, 32*2, TailStrategy::GuardWithIf);
    //     //in.unroll(tx, 2);
    //     in.split(tx, tx, ix, 2);
    //     in.gpu(bx, tx);
    //     in.update(0)
    //       .split(x, bx, tx, 32*4, TailStrategy::GuardWithIf)
    //       .unroll(tx, 2)
    //       .gpu(bx, tx)
    //       ;

    //     h.compute_at(in, bx).gpu_threads(x);
    //     h.update().reorder(r.x, x).gpu_threads(x);
        
    //     k.compute_root();


    //     f.compute_root()
    //      .fuse(x, y, xy)
    //      .split(xy, bx, tx, 32, TailStrategy::GuardWithIf)
    //      .gpu(bx, tx)
    //      ;

    //     f.update(0)
    //      .split(x, bx, tx, 32, TailStrategy::GuardWithIf)
    //      .gpu(bx, tx)
    //      ;

    //     //f.update(0).compute_root();

        
        

        
    // } else if(true) {
    //     even.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
    //     even.bound(x,0,127);
    //     even.output_buffer().dim(0).set_min(0);
    //     in.compute_at(even, bx);
    // } else {
    //     even.gpu_tile(x, bx, tx, 32, TailStrategy::GuardWithIf);
    //     even.bound(x,0,127);
    //     even.output_buffer().dim(0).set_min(0);
    // }
    
    // //in.gpu_tile(x, bx, tx, 2); // Divides extent
    // //even.gpu_tile(x, bx, tx, 3); // One thread block

    // Target target = get_host_target();
    // Target new_target = target
    //   .with_feature(Target::OpenCL)
    //   .with_feature(Target::NoAsserts)
    //   ;
    // assert(host_supports_target_device(new_target));
    // if(add_runtime){
    //     compile_standalone_runtime("opencl_runtime.o",new_target);
    //     even.compile_to_header("even_c.h", {}, "even", new_target);
    // }

    // if(!add_runtime && !aot)
    //     even.compile_to_c("even_c.cpp", {}, "even", new_target);

    // if(aot)
    //     even.compile_to_static_library("even_static", {}, "even", new_target);

    // if(false)
    // {
    //     //Sum try out
    //     Func f("f"), g("g"), g_int("g_int");
    //     Var i("i"), threads("threads"), blocks("blocks");
    //     RDom r(0, 256);
    //     f(i) = i*i*2;

    //     RVar r_blocks("r_blocks"), r_temp, r_threads("r_threads"), r_i("r_i");
    //     g() = 0;
    //     g() += f(r.x);

    //     f.compute_root();
    //     g.update()
    //         .split(r.x, r_blocks, r_temp, 32*4)
    //         .split(r_temp, r_threads, r_i, 4)
    //         ;
            
    //     g.update()
    //         .rfactor({{r_blocks, blocks}, {r_threads, threads}})
    //         // .gpu(blocks, threads)
    //         .compute_root()
    //         .update()
    //         .gpu(blocks, threads)
    //         ;

    //     g.compile_to_c("sum_c.cpp", {}, "sum", new_target);

    //     // Func histogram("hist_serial");
    //     // histogram(i) = 0;
    //     // RDom r(0, input.width(), 0, input.height());
    //     // histogram(input(r.x, r.y) / 32) += 1;

    // }

}