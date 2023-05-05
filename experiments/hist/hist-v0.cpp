#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
    int schedule;
    std::string front_s = "front";
    bool front = false;
    if(argc == 1){
        schedule = 0;
    } else if(argc == 2){
        if(front_s.compare(argv[1]) == 0){
        front = true;
        } else {
        schedule = std::stoi(argv[1]);
        }
    }
    if((argc > 2 || schedule < 0 || schedule >4) && !front){
        printf("Invallid argument\n");
        return 1;
    }

    int nx = 1536;
    int ny = 2560;

    ImageParam input(type_of<int>(), 3, "input");
    Var x("x"), y("y"), c("c");
    // Algorithm
    Func Y("Y");
    Y(x, y) = (299 * input(x, y, 0) +
               587 * input(x, y, 1) +
               114 * input(x, y, 2))/1000;
    Func Cr("Cr");
    Expr R = input(x, y, 0);
    Cr(x, y) = ((R - Y(x, y)) * 713) / 1000 + 128;
    Func Cb("Cb");
    Expr B = input(x, y, 2);
    Cb(x, y) = ((B - Y(x, y)) * 564) / 1000 + 128;
    Func hist_rows("hist_rows");
    hist_rows(x, y) = 0;
    hist_rows.ensures(hist_rows(x, y) == 0);
    RDom rx(0, nx);
    Expr bin = clamp(Y(rx, y), 0, 255);
    hist_rows(bin, y) += 1;
    hist_rows.invariant(hist_rows(x,y) <= rx);
    hist_rows.ensures(hist_rows(x,y) <= nx);
    Func hist("hist");
    hist(x) = 0;
    hist.ensures(hist(x) == 0);
    RDom ry(0, ny);
    hist(x) += hist_rows(x, ry);
    hist.invariant(hist(x) <= ry*nx);
    hist.ensures(hist(x) <= ny*nx);
    Func cdf("cdf");
    cdf(x) = hist(0);
    RDom b(1, 255);
    cdf(b.x) = cdf(b.x - 1) + hist(b.x);
    Func cdf_bin("cdf_bin");
    cdf_bin(x, y) = clamp(Y(x, y), 0, 255);
    Func eq("equalize");
    eq(x, y) = clamp((cdf(cdf_bin(x, y)) * 255) / (ny * nx), 0, 255);
    Expr red = clamp((eq(x, y)*1000 + (Cr(x, y) - 128) * 1400)/1000, 0, 255);
    Expr green = clamp((eq(x, y)*1000 - 343 * (Cb(x, y) - 128) - 711 * (Cr(x, y) - 128))/1000, 0, 255);
    Expr blue = clamp((eq(x, y)*1000 + 1765 * (Cb(x, y) - 128))/1000, 0, 255);
    Func output("output");
    // output(x, y, c) = mux(c, {red, green, blue});
    output(x, y, c) = mux(c, {red, Y(x,y), Cb(x,y)});
    // Estimates (for autoscheduler; ignored otherwise)
    {
        input.dim(0).set_bounds(0, 1536);
        input.dim(1).set_bounds(0, 2560);
        input.dim(2).set_bounds(0, 3);
        input.dim(1).set_stride(1536);
        input.dim(2).set_stride(1536*2560);

        output.output_buffer().dim(0).set_bounds(0, 1536);
        output.output_buffer().dim(1).set_bounds(0, 2560);
        output.output_buffer().dim(2).set_bounds(0, 3);
        output.output_buffer().dim(1).set_stride(1536);
        output.output_buffer().dim(2).set_stride(1536*2560);
    }

    cdf.bound(x, 0, 256);
    output.bound(c, 0, 3);

    // Schedule
    if(schedule == 0){

    } else if(schedule == 1){

        eq.compute_at(output, y);

        output.reorder(c, x, y)
            .unroll(c)
            .parallel(y)
            ;
    } else if(schedule == 2){
        Y.compute_root();
        eq.compute_root();
        hist.compute_root();

        output
            .reorder(c, x, y)
            .unroll(c)
            .parallel(y)
            ;
    } else if(schedule == 3){
        int vec = 4;
        Y
            .clone_in(hist_rows)
            .compute_at(hist_rows.in(), y)
            // .vectorize(x, vec)
            ;

        hist_rows.in()
            .compute_root()
            // .vectorize(x, vec)
            .parallel(y, 4)
            ;
        hist_rows.compute_at(hist_rows.in(), y)
            // .vectorize(x, vec)
            .update()
            .reorder(y, rx)
            // .unroll(y)
            ;
        hist.compute_root()
            // .vectorize(x, vec)
            .update()
            .reorder(x, ry)
            // .vectorize(x, vec)
            // .unroll(x, 4)
            .parallel(x)
            .reorder(ry, x)
            ;

        cdf.compute_root();
        output.reorder(c, x, y)
            .unroll(c)
            .parallel(y, 8)
            // .vectorize(x, vec * 2)
            // .unroll(x, vec*2)
            ;
    } else if(schedule == 4){
        const int vec = 4; ///natural_vector_size<float>();
        // Make separate copies of Y to use while
        // histogramming and while computing the output. It's
        // better to redundantly luminance than reload it, but
        // you don't want to inline it into the histogram
        // computation because then it doesn't vectorize.
        RVar rxo("rxo"), rxi("rxi");
        Var  v("v"), yi("yi");
        Y.clone_in(hist_rows)
            .compute_at(hist_rows.in(), y)
            .split(x, x, v, vec)
            // .vectorize(v)
            ;

        hist_rows.in()
            .compute_root()
            .split(x, x, v, vec)
            // .vectorize(v)
            .split(y, y, yi, 4)
            .parallel(y)
            ;
        hist_rows.compute_at(hist_rows.in(), y)
            .split(x, x, v, vec)
            // .vectorize(v)
            .update()
            .reorder(y, rx)
            .unroll(y)
            ;
        hist.compute_root()
            .split(x, x, v, vec)
            // .vectorize(v)
            .update()
            .reorder(x, ry)
            .split(x, x, v, vec)
            // .vectorize(v)
            .unroll(x, 4)
            .parallel(x)
            .reorder(ry, x)
            ;

        cdf.compute_root();
        output.reorder(c, x, y)
            .bound(c, 0, 3)
            .unroll(c)
            .split(y, y, yi, 8)
            .parallel(y)
            .split(x, x, v, vec * 2)
            // .vectorize(v)
            ;

    }

    Target target = Target();
    Target new_target = target
      .with_feature(Target::NoAsserts)
      .with_feature(Target::NoBoundsQuery)
      ;

    std::string name = "hist";

    if(front) {
        output.translate_to_pvl(name + "_front_pvl.pvl", {}, {}); 
    } else {
        output.compile_to_pvl(name + "_pvl-" + std::to_string(schedule) + ".pvl" , {input}, {}, name, new_target);
    }
}