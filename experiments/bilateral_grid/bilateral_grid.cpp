#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
    // ImageParam input(type_of<int>(), 2, "input");
    ImageParam input(type_of<float>(), 2, "input");

    int s_sigma = 8;
    float r_sigma = 0.1f;
    // r_sigma = 10;

    // Output<Buffer<float>> bilateral_grid{"bilateral_grid", 2};
    Func bilateral_grid("bilateral_grid");
    OutputImageParam output;

    Var x("x"), y("y"), z("z"), c("c"); 

    // Add a boundary condition
    Func clamped = BoundaryConditions::repeat_edge(input);

    // Construct the bilateral grid
    RDom r(0, s_sigma, 0, s_sigma);
    Expr val = clamp(clamped(x * s_sigma + r.x - s_sigma / 2, y * s_sigma + r.y - s_sigma / 2), 0.0f, 1.0f);
    val = clamp(val, 0.0f, 1.0f);

    Expr zi = cast<int>(val * (1.0f / r_sigma) + 0.5f);

    Func histogram("histogram");
    histogram(x, y, z, c) = 0.0f;
    histogram(x, y, zi, c) += mux(c, {val, 1.0f});

    // Blur the grid using a five-tap filter
    Func blurx("blurx"), blury("blury"), blurz("blurz");
    blurz(x, y, z, c) = (histogram(x, y, z - 2, c) +
                         histogram(x, y, z - 1, c) * 4 +
                         histogram(x, y, z, c) * 6 +
                         histogram(x, y, z + 1, c) * 4 +
                         histogram(x, y, z + 2, c));
    blurx(x, y, z, c) = (blurz(x - 2, y, z, c) +
                         blurz(x - 1, y, z, c) * 4 +
                         blurz(x, y, z, c) * 6 +
                         blurz(x + 1, y, z, c) * 4 +
                         blurz(x + 2, y, z, c));
    blury(x, y, z, c) = (blurx(x, y - 2, z, c) +
                         blurx(x, y - 1, z, c) * 4 +
                         blurx(x, y, z, c) * 6 +
                         blurx(x, y + 1, z, c) * 4 +
                         blurx(x, y + 2, z, c));

    // Take trilinear samples to compute the output
    val = clamp(input(x, y), 0.0f, 1.0f);
    Expr zv = val * (1.0f / r_sigma);
    zi = cast<int>(zv);
    Expr zf = zv - zi;
    Expr xf = cast<float>(x % s_sigma) / s_sigma;
    Expr yf = cast<float>(y % s_sigma) / s_sigma;
    Expr xi = x / s_sigma;
    Expr yi = y / s_sigma;
    Func interpolated("interpolated");
    interpolated(x, y, c) =
        lerp(lerp(lerp(blury(xi, yi, zi, c), blury(xi + 1, yi, zi, c), xf),
                  lerp(blury(xi, yi + 1, zi, c), blury(xi + 1, yi + 1, zi, c), xf), yf),
             lerp(lerp(blury(xi, yi, zi + 1, c), blury(xi + 1, yi, zi + 1, c), xf),
                  lerp(blury(xi, yi + 1, zi + 1, c), blury(xi + 1, yi + 1, zi + 1, c), xf), yf),
             zf);

    // Normalize
    bilateral_grid(x, y) = interpolated(x, y, 0) / interpolated(x, y, 1);

    output = bilateral_grid.output_buffer();

    int nx = 1536;
    int ny = 2560;
    /* Bounds */
    input.dim(0).set_bounds(0,nx);
    input.dim(1).set_bounds(0,ny);
    input.dim(1).set_stride(nx);
    histogram.bound(z, -2, 16);
    blurz.bound(z, 0, 12);
    blurx.bound(z, 0, 12);
    blury.bound(z, 0, 12);
    output.dim(0).set_bounds(0, nx);
    output.dim(1).set_bounds(0, ny);
    output.dim(1).set_stride(nx);

    // CPU schedule.

    // 3.98ms on an Intel i9-9960X using 32 threads at 3.7 GHz
    // using target x86-64-avx2. This is a little less
    // SIMD-friendly than some of the other apps, so we
    // benefit from hyperthreading, and don't benefit from
    // AVX-512, which on my machine reduces the clock to 3.0
    // GHz.

    blurz.compute_root()
        .reorder(c, z, x, y)
        .parallel(y)
        // .vectorize(x, 8)
        .unroll(c);
    histogram.compute_at(blurz, y);
    histogram.update()
        .reorder(c, r.x, r.y, x, y)
        .unroll(c);
    blurx.compute_root()
        .reorder(c, x, y, z)
        .parallel(z)
        // .vectorize(x, 8)
        .unroll(c);
    blury.compute_root()
        .reorder(c, x, y, z)
        .parallel(z)
        // .vectorize(x, 8)
        .unroll(c);
    bilateral_grid.compute_root()
        .parallel(y)
        // .vectorize(x, 8)
        ;

    Target target = Target();
    Target new_target = target
    .with_feature(Target::NoAsserts)
    .with_feature(Target::NoBoundsQuery)
    ;

    std::string name = "bilateral_grid";

    bilateral_grid.compile_to_pvl(name + "_pvl.pvl", {input}, {}, name, new_target);
    // bilateral_grid.translate_to_pvl(name + "_front_pvl.pvl", {}, {}); 

}

