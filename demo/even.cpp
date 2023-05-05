#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
    Func f("f"), g("g"), j("j"), out("out");
    Var x("x"), y("y"), i("i");
    Var xi("xi"), xo("xo"), xy("xy"), yi("yi");
    ImageParam inp(type_of<int>(), 1, "inp");
    ImageParam input(type_of<int>(), 2, "input");
    RDom r(0,3, "r");

    RDom s(0,3,0,4, "s");

    Expr sumj = Internal::Call::make(Int(32), "sumj", {i}, Internal::Call::PureExtern);


    // inp.requires(inp(_) % 2 == 0);
    inp.requires(inp(_) > 0);
    // inp.requires(forall(x, x>=0 && x<6 ,inp(x) % 2 == 0));

    f(x,y) = inp(x) + inp(y);
    f.ensures(f(x,y) == inp(x) + inp(y));
        
    f(x,0) = f(x,0) + f(x,1);
    // f.ensures(f(x,y) % 2 == 0);

    // g(x,y) = 0.0f;
    // g(x,y) = g(x,y) + f(x+r, y);
    // g.invariant(implies(r==0, g(x,y) == 0));
    // g.invariant(implies(r>0, g(x,y) > 0));
    // g.invariant(g(x,y) % 2 == 0);
    // g.ensures(g(x,y) % 2 == 0);

    // g(x,y) = f(x,y)+ f(x+1,y) + f(x+2,y);
    // g.ensures(g(x,y) % 2 == 0);

    // j(x) = x + 1;
    // g(x,y) = 0;
    // g(x,y) += f(x+r, y);
    // g(x,r) = g(x,r-1) + j(x+r);
    // g.invariant(implies(r == 0, g(x,r) == 0));
    // g.invariant(forall(i, x==0 && y==i && r>=i+1 && 0<=i && i<4, g(x,r) == sumj ));

    g(x,y) = 0;
    g(s.x,s.y) = g(s.x-1, s.y-1) + 1;
    g.invariant(implies(s.x==0 && s.y==0, g(s.x,s.y)==0));
    g.invariant(implies(x==-1 && y==-1, g(s.x,s.y)==0));
    g.invariant(implies(s.x>0 && s.y==1 && x==0 && y==0, g(s.x,s.y)==1));
    g.invariant(implies(s.y>1 && x==0 && y==0, g(s.x,s.y)==1));
    g.invariant(implies(s.x>1 && s.y==2 && x==1 && y==1, g(s.x,s.y)==2));


    // g.invariant(g(x,r) == sumj);
    // g.invariant(forall(i, x==i, g(x,r) > 0));
    // g.invariant()

    out(x,y) = g(x,y);
    // out.ensures(out(x,y) % 2 == 0);
    out.ensures(out(x,y) > 0);

    out.compute_root();
    g.compute_root();

    

    int option = 8;
    switch (option){
      case 1:
        f.compute_root();
        break;
      case 2:
        f.compute_at(g,y);
        break;
      case 3:
        f.compute_at(g,x);
          break;
      case 4:
        // Almost similar to compute_root
        f.compute_at(g,Var::outermost());
        break;
      case 5:
        g.split(x, xo, xi, 2, TailStrategy::GuardWithIf);
        g.unroll(xi);
        f.compute_at(g,y);
        break; 
      case 6:
        // Works with latest VerCors
        g.split(x, xo, xi, 3, TailStrategy::GuardWithIf);
        f.compute_at(g,y);
        break;
      case 7:
        // VerCors cannot verify this yet, because it does not support '&' and '<<' bit operators
        g.fuse(x,y, xy);
        break;
      case 8:
        // This doesn't work due to VerCors limitations (nested forall's)
        g.reorder(y,x);
        break;
      case 9:
        // Not supported yet, but should work soon (minor adjustments)
        g.parallel(y);
        break;
      case 10:
        g.split(x, x, xo, 3, TailStrategy::GuardWithIf);
        g.split(xo, xo, xi, 2, TailStrategy::GuardWithIf);
    }

    int out_x = 8;
    int out_y = 3;
    
    out.output_buffer().dim(0).set_bounds(0, out_x);
    out.output_buffer().dim(1).set_bounds(0, out_y);
    out.output_buffer().dim(1).set_stride(out_x);
    inp.dim(0).set_bounds(0,std::max(out_x+2, out_y));
    
    Target target = get_host_target();
    Target new_target = target
      .with_feature(Target::OpenCL)
      .with_feature(Target::NoAsserts)
      .with_feature(Target::NoBoundsQuery)
      ;
    assert(host_supports_target_device(new_target));

    // out.print_loop_nest();

    // out.compile_to_pvl("even_pvl.pvl", {inp}, "even", new_target);

    // out.compile_to_c("even_c.cpp", {inp}, "even", new_target);

    // out.translate_to_pvl("even_front.pvl", {inp});


    Func blur_x("blur_x"), blur_y("blur_y");
    // Var x, y, xi, yi;

    // The algorithm - no storage or order
    blur_x(x, y) = (input(x-1, y) + input(x, y) + input(x+1, y));
    blur_x.ensures(blur_x(x, y) == input(x-1, y) + input(x, y) + input(x+1, y));
    blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/9;
    blur_y.ensures(blur_y(x, y) == 
      (input(x-1, y-1) + input(x, y-1) + input(x+1, y-1)
      + input(x-1, y) + input(x, y) + input(x+1, y)
      + input(x-1, y+1) + input(x, y+1) + input(x+1, y+1)
      ) / 9
    );

    // The schedule - defines order, locality; implies storage
    blur_y.tile(x, y, xi, yi, 256, 32)
          .vectorize(xi, 8).parallel(y);
    blur_x.compute_at(blur_y, x).vectorize(x, 8);

    int nx = 4096;
    int ny = 4096;

    blur_y.output_buffer().dim(0).set_bounds(1, nx);
    blur_y.output_buffer().dim(1).set_bounds(1, ny);
    blur_y.output_buffer().dim(1).set_stride(nx);
    input.dim(0).set_bounds(0,nx+2);
    input.dim(1).set_bounds(0,ny+2);
    input.dim(1).set_stride(nx+2);
 
    blur_y.compile_to_c("blur_c.cpp", {input}, "blur", new_target);
    blur_y.compile_to_pvl("blur_pvl.pvl", {input}, "blur", new_target);
    blur_y.translate_to_pvl("blur_front_pvl.pvl", {});

    RDom rr(1,10,"r");
    Func sum("sum");
    sum(x) = 0;
    sum.ensures(sum(x) == 0);
    sum(rr) = sum(rr-1) + 1;
    sum.invariant(1<=r && r <=11);
    sum.invariant(implies(0<=x && x<rr, sum(x) == x));
    sum.ensures(implies(0<=x && x<11, sum(x) == x));
    // sum.ensures(implies(1<=x && x<=11, sum(x) == x) && implies(!(1<=x && x<=11), sum(x) == 0));
    // sum.compile_to_pvl("sum_pvl.pvl", {} , "sum", new_target);
    sum.translate_to_pvl("sum_front_pvl.pvl", {});

}