#include "Halide.h"
#include <stdio.h>

using namespace Halide;

Var x("x"), y("y"), z("z"), c("c");

// A class to hold a matrix of Halide Exprs.
template<int rows, int cols>
struct Matrix {
    Expr exprs[rows][cols];

    Expr operator()(int i, int j) const {
        return exprs[i][j];
    }

    Expr &operator()(int i, int j) {
        return exprs[i][j];
    }

    void dump() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                std::cout << exprs[i][j];
                if (j < cols - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }
};

// Matrix-matrix multiply.
template<int R, int S, int T>
Matrix<R, T> mat_mul(const Matrix<R, S> &A, const Matrix<S, T> &B) {
    Matrix<R, T> result;
    for (int r = 0; r < R; r++) {
        for (int t = 0; t < T; t++) {
            result(r, t) = 0.0f;
            for (int s = 0; s < S; s++) {
                result(r, t) += A(r, s) * B(s, t);
            }
        }
    }
    return result;
}

// Solve Ax = b at each x, y, z. Compute the result at the given Func
// and Var. Not currently used, but preserved for reference.
template<int M, int N>
Matrix<M, N> solve(Matrix<M, M> A, Matrix<M, N> b, Func compute, Var at, bool skip_schedule, Target target) {
    // Put the input matrices in a Func to do the Gaussian elimination.
    Var vi, vj;
    Func f;
    f(x, y, z, vi, vj) = undef<float>();
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            f(x, y, z, i, j) = A(i, j);
        }
        for (int j = 0; j < N; j++) {
            f(x, y, z, i, j + M) = b(i, j);
        }
    }

    // eliminate lower left
    for (int k = 0; k < M - 1; k++) {
        for (int i = k + 1; i < M; i++) {
            f(x, y, z, -1, 0) = f(x, y, z, i, k) / f(x, y, z, k, k);
            for (int j = k + 1; j < M + N; j++) {
                f(x, y, z, i, j) -= f(x, y, z, k, j) * f(x, y, z, -1, 0);
            }
            f(x, y, z, i, k) = 0.0f;
        }
    }

    // eliminate upper right
    for (int k = M - 1; k > 0; k--) {
        for (int i = 0; i < k; i++) {
            f(x, y, z, -1, 0) = f(x, y, z, i, k) / f(x, y, z, k, k);
            for (int j = k + 1; j < M + N; j++) {
                f(x, y, z, i, j) -= f(x, y, z, k, j) * f(x, y, z, -1, 0);
            }
            f(x, y, z, i, k) = 0.0f;
        }
    }

    // Divide by diagonal and put it in the output matrix.
    for (int i = 0; i < M; i++) {
        f(x, y, z, i, i) = 1.0f / f(x, y, z, i, i);
        for (int j = 0; j < N; j++) {
            b(i, j) = f(x, y, z, i, j + M) * f(x, y, z, i, i);
        }
    }

    if (!skip_schedule) {
        if (!target.has_gpu_feature()) {
            for (int i = 0; i < f.num_update_definitions(); i++) {
                f.update(i).vectorize(x);
            }
        }

        f.compute_at(compute, at);
    }

    return b;
};

// Solve Ax = b at each x, y, z exploiting the fact that A is
// symmetric. Compute the result at the given Func and Var.
template<int M, int N>
Matrix<M, N> solve_symmetric(Matrix<M, M> A_, Matrix<M, N> b_,
                             Func compute, Var at, bool skip_schedule /*, Target target*/) {

    // Put the input matrices in a Func to do sqrt-free Cholesky.
    // See https://users.wpi.edu/~walker/MA514/HANDOUTS/cholesky.pdf
    // for an explanation of sqrt-free Cholesky.

    Var vi, vj;
    Func f;
    f(x, y, z, vi, vj) = undef<float>();

    // Add more usefully-named accessors.
    auto A = [&](int i, int j) {
        return f(x, y, z, i, j);
    };
    auto b = [&](int i, int j) {
        return f(x, y, z, i, M + j);
    };

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            A(i, j) = A_(i, j);
        }
        for (int j = 0; j < N; j++) {
            b(i, j) = b_(i, j);
        }
    }

    // L D L' factorization, packed into a single matrix. We'll store
    // L in the lower triangle, 1/D on the diagonal, and ??? TODO in
    // the upper triangle.
    for (int j = 0; j < M; j++) {
        // Normalize the jth column starting at the jth row, storing
        // the normalization factor on the diagonal. Because A(i, j)
        // is symmetric, the unnormalized version stays in the jth
        // row.
        A(j, j) = fast_inverse(A(j, j));
        for (int i = j + 1; i < M; i++) {
            A(i, j) *= A(j, j);
        }

        // Subtract the outer product of the jth column with its
        // unnormalized version from the rest of the matrix down and
        // to the right of it.
        for (int i = j + 1; i < M; i++) {
            for (int k = j + 1; k < M; k++) {
                if (k < i) {
                    // We already did this one. Exploit symmetry
                    A(i, k) = A(k, i);
                } else {
                    A(i, k) -= A(k, j) * A(j, i);
                }
            }
        }
    }

    // We're done with the upper (unnormalized) triangle
    // now.

    // Back substitute to solve:
    // LDL' x = b
    // We're going to peel the matrices off the left-hand-side from
    // left to right, updating b as we go.
    Matrix<M, N> result;
    for (int k = 0; k < N; k++) {
        // First remove the leftmost L, by solving Lz = b.
        for (int j = 0; j < M; j++) {
            for (int i = 0; i < j; i++) {
                b(j, k) -= A(j, i) * b(i, k);
            }
        }
        // L has a unit diagonal, so there's no scaling step

        // The problem is now DL' x = b

        // Multiply both sizes by D inverse, which we have stored on the
        // diagonal.
        for (int j = 0; j < M; j++) {
            b(j, k) *= A(j, j);
        }

        // The problem is now L' x = b

        // Multiply both sides by L transpose inverse.
        for (int j = M - 1; j >= 0; j--) {
            for (int i = j + 1; i < M; i++) {
                b(j, k) -= A(i, j) * b(i, k);
            }
        }

        for (int j = 0; j < M; j++) {
            result(j, k) = b(j, k);
        }
    }

    if (!skip_schedule) {
        // if (!target.has_gpu_feature()) {
        //     for (int i = 0; i < f.num_update_definitions(); i++) {
        //         f.update(i).vectorize(x);
        //     }
        // }

        f.compute_at(compute, at);
    }

    return result;
}

template<int N, int M>
Matrix<M, N> transpose(const Matrix<N, M> &in) {
    Matrix<M, N> out;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            out(j, i) = in(i, j);
        }
    }
    return out;
}

Expr pack_channels(Var c, std::vector<Expr> exprs) {
    Expr e = exprs.back();
    for (int i = (int)exprs.size() - 2; i >= 0; i--) {
        e = select(c == i, exprs[i], e);
    }
    return e;
}


int main(int argc, char *argv[]) {
    float r_sigma = 1.0f/8.0f;
    int s_sigma = 16;

    ImageParam splat_loc(type_of<float>(), 3, "splat_loc");
    ImageParam values(type_of<float>(), 3, "values");
    ImageParam slice_loc(type_of<float>(), 3, "slice_loc");
    // r_sigma.set_estimate(1.f / 8.f);
    // s_sigma.set_estimate(16.f);
    splat_loc.dim(0).set_bounds(0, 192);
    splat_loc.dim(1).set_bounds(0, 320);
    splat_loc.dim(2).set_bounds(0, 3);
    values.dim(0).set_bounds(0, 192);
    values.dim(1).set_bounds(0, 320);
    values.dim(2).set_bounds(0, 3);
    slice_loc.dim(0).set_bounds(0, 1536);
    slice_loc.dim(1).set_bounds(0, 2560);
    slice_loc.dim(2).set_bounds(0, 3);
    

    Expr upsample_factor_x = 1536 / 192;
    Expr upsample_factor_y = 2560 / 320;
    Expr upsample_factor = max(upsample_factor_x, upsample_factor_y);

    Func clamped_values = BoundaryConditions::repeat_edge(values);
    Func clamped_splat_loc = BoundaryConditions::repeat_edge(splat_loc);

    Func gray_splat_loc;
    gray_splat_loc(x, y) = (0.25f * clamped_splat_loc(x, y, 0) +
                            0.5f * clamped_splat_loc(x, y, 1) +
                            0.25f * clamped_splat_loc(x, y, 2));

    Func gray_slice_loc;
    gray_slice_loc(x, y) = (0.25f * slice_loc(x, y, 0) +
                            0.5f * slice_loc(x, y, 1) +
                            0.25f * slice_loc(x, y, 2));

    Func histogram("histogram");
    RDom r(0, s_sigma, 0, s_sigma);
    {
        histogram(x, y, z, c) = 0.0f;

        Expr sx = x * s_sigma + r.x - s_sigma / 2, sy = y * s_sigma + r.y - s_sigma / 2;
        Expr pos = gray_splat_loc(sx, sy);
        pos = clamp(pos, 0.0f, 1.0f);
        Expr zi = cast<int>(round(pos * (1.0f / r_sigma)));

        // Sum all the terms we need to fit a line relating
        // low-res input to low-res output within this bilateral grid
        // cell
        Expr vr = clamped_values(sx, sy, 0), vg = clamped_values(sx, sy, 1), vb = clamped_values(sx, sy, 2);
        Expr sr = clamped_splat_loc(sx, sy, 0), sg = clamped_splat_loc(sx, sy, 1), sb = clamped_splat_loc(sx, sy, 2);

        histogram(x, y, zi, c) +=
            pack_channels(c,
                        {sr * sr, sr * sg, sr * sb, sr,
                        sg * sg, sg * sb, sg,
                        sb * sb, sb,
                        1.0f,
                        vr * sr, vr * sg, vr * sb, vr,
                        vg * sr, vg * sg, vg * sb, vg,
                        vb * sr, vb * sg, vb * sb, vb});
    }

    Expr t0 = 1.0f / 64, t1 = 1.0f / 27, t2 = 1.0f / 8, t3 = 1.0f;

    Func blurx("blurx"), blury("blury"), blurz("blurz");

    blurz(x, y, z, c) = (histogram(x, y, z - 3, c) * t0 +
                            histogram(x, y, z - 2, c) * t1 +
                            histogram(x, y, z - 1, c) * t2 +
                            histogram(x, y, z, c) * t3 +
                            histogram(x, y, z + 1, c) * t2 +
                            histogram(x, y, z + 2, c) * t1 +
                            histogram(x, y, z + 3, c) * t0);
    blury(x, y, z, c) = (blurz(x, y - 3, z, c) * t0 +
                            blurz(x, y - 2, z, c) * t1 +
                            blurz(x, y - 1, z, c) * t2 +
                            blurz(x, y, z, c) * t3 +
                            blurz(x, y + 1, z, c) * t2 +
                            blurz(x, y + 2, z, c) * t1 +
                            blurz(x, y + 3, z, c) * t0);
    blurx(x, y, z, c) = (blury(x - 3, y, z, c) * t0 +
                            blury(x - 2, y, z, c) * t1 +
                            blury(x - 1, y, z, c) * t2 +
                            blury(x, y, z, c) * t3 +
                            blury(x + 1, y, z, c) * t2 +
                            blury(x + 2, y, z, c) * t1 +
                            blury(x + 3, y, z, c) * t0);

    Func line("line");
    {
        // Pull out the 4x4 symmetric matrix from the values we've
        // accumulated.
        Matrix<4, 4> A;
        A(0, 0) = blurx(x, y, z, 0);
        A(0, 1) = blurx(x, y, z, 1);
        A(0, 2) = blurx(x, y, z, 2);
        A(0, 3) = blurx(x, y, z, 3);
        A(1, 0) = A(0, 1);
        A(1, 1) = blurx(x, y, z, 4);
        A(1, 2) = blurx(x, y, z, 5);
        A(1, 3) = blurx(x, y, z, 6);
        A(2, 0) = A(0, 2);
        A(2, 1) = A(1, 2);
        A(2, 2) = blurx(x, y, z, 7);
        A(2, 3) = blurx(x, y, z, 8);
        A(3, 0) = A(0, 3);
        A(3, 1) = A(1, 3);
        A(3, 2) = A(2, 3);
        A(3, 3) = blurx(x, y, z, 9);

        // Pull out the rhs
        Matrix<4, 3> b;
        b(0, 0) = blurx(x, y, z, 10);
        b(1, 0) = blurx(x, y, z, 11);
        b(2, 0) = blurx(x, y, z, 12);
        b(3, 0) = blurx(x, y, z, 13);
        b(0, 1) = blurx(x, y, z, 14);
        b(1, 1) = blurx(x, y, z, 15);
        b(2, 1) = blurx(x, y, z, 16);
        b(3, 1) = blurx(x, y, z, 17);
        b(0, 2) = blurx(x, y, z, 18);
        b(1, 2) = blurx(x, y, z, 19);
        b(2, 2) = blurx(x, y, z, 20);
        b(3, 2) = blurx(x, y, z, 21);

        // Regularize by pushing the solution towards the average gain
        // in this cell = (average output luma + eps) / (average input luma + eps).
        const float lambda = 1e-6f;
        const float epsilon = 1e-6f;

        // The bottom right entry of A is a count of the number of
        // constraints affecting this cell.
        Expr N = A(3, 3);

        // The last row of each matrix is the sum of input and output
        // RGB values for the pixels affecting this cell. Instead of
        // dividing them by N+1 to get averages, we'll multiply
        // epsilon by N+1. This saves two divisions.
        Expr output_luma = b(3, 0) + 2 * b(3, 1) + b(3, 2) + epsilon * (N + 1);
        Expr input_luma = A(3, 0) + 2 * A(3, 1) + A(3, 2) + epsilon * (N + 1);
        Expr gain = output_luma / input_luma;

        // Add lambda and lambda*gain to the diagonal of the
        // matrices. The matrices are sums/moments rather than
        // means/covariances, so just like above we need to multiply
        // lambda by N+1 so that it's equivalent to adding a constant
        // to the diagonal of a covariance matrix. Otherwise it does
        // nothing in cells with lots of linearly-dependent
        // constraints.
        Expr weighted_lambda = lambda * (N + 1);
        A(0, 0) += weighted_lambda;
        A(1, 1) += weighted_lambda;
        A(2, 2) += weighted_lambda;
        A(3, 3) += weighted_lambda;

        b(0, 0) += weighted_lambda * gain;
        b(1, 1) += weighted_lambda * gain;
        b(2, 2) += weighted_lambda * gain;

        // Now solve Ax = b
        Matrix<3, 4> result = transpose(solve_symmetric(A, b, line, x, false));

        // Pack the resulting matrix into the output Func.
        line(x, y, z, c) = pack_channels(c, {result(0, 0),
                                                result(0, 1),
                                                result(0, 2),
                                                result(0, 3),
                                                result(1, 0),
                                                result(1, 1),
                                                result(1, 2),
                                                result(1, 3),
                                                result(2, 0),
                                                result(2, 1),
                                                result(2, 2),
                                                result(2, 3)});
    }

    Target target = Target();
    Target new_target = target
        .with_feature(Target::NoAsserts)
        .with_feature(Target::NoBoundsQuery)
        ;

    OutputImageParam output = line.output_buffer();
    output.dim(0).set_bounds(0, 1536);
    output.dim(1).set_bounds(0, 2560);
    output.dim(2).set_bounds(0, 3);

    // line.compile_to_pvl("bgu_pvl.pvl", {splat_loc, values, slice_loc}, "bgu", new_target);
    line.translate_to_pvl("bgu_front_pvl.pvl", {}, {}); 
}