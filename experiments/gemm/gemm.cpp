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
  if((argc > 2 || schedule < 0 || schedule >3) && !front){
    printf("Invallid argument\n");
    return 1;
  }

  ImageParam A_(type_of<int>(), 2, "A_");
  ImageParam B_(type_of<int>(), 2, "B_");
  ImageParam C_(type_of<int>(), 2, "C_");

  A_.requires(0 <= A_(_0, _1) && A_(_0, _1) < 100);
  B_.requires(0 <= B_(_0, _1) && B_(_0, _1) < 100);
  C_.requires(0 <= C_(_0, _1) && C_(_0, _1) < 100);

  const int num_rows = 2048;
  const int num_cols = 2048;
  const int sum_size = 1024;
  const int a_ = 2;
  const int b_ = 3;


  const int vec = 4;
  const int s = vec * 2;
  Var i("i"), j("j"), ii("ii"), ji("ji"), jii("jii"), iii("iii"), io("io"), jo("jo"), t;
  Var ti[3], tj[3];

        // Swizzle A for better memory order in the inner loop.
  Func A("A"), B("B"), Btmp("Btmp"), As("As"), Atmp("Atmp"), result_("_result");
  // Atmp(i, j) //BoundaryConditions::constant_exterior(A_, 0)(i, j);
  //   = select(i >= 0 && i < num_rows && j >=0 && j < sum_size, A_(i,j), 0);
  Atmp(i, j) = A_(i,j);
  // i : [0,10], j : [0,4]
  // As[0,0,0] = Atmp[0,0]
  // A[9, 1] := As[1, 1, 1] := Atmp[8*1+1, 1]
  As(i, j, io) = Atmp(io * s + i, j);
  As.ensures(implies(io >=0 && io < num_rows/s && i >= 0 && i < num_rows && j >=0 && j < sum_size, As(i, j, io) == A_(io * s + i, j)));
  A(i, j) = As(i % s, j, i / s);
  A.ensures(implies(i >= 0 && i < num_rows && j >=0 && j < sum_size, A(i, j) == A_(i,j)));
  B(i, j) = B_(i, j);

  // 2048/8 = 

  Var k("k");
  Func prod("prod");
  // Express all the products we need to do a matrix multiply as a 3D Func.
  prod(k, i, j) = A(i, k) * B(k, j);

  // Reduce the products along k.
  Func AB("AB");
  AB(i, j) = 0;
  AB.ensures(AB(i,j) == 0);
  RDom rv(0, sum_size);
  AB(i, j) += prod(rv, i, j);
  AB.loop_invariant(AB(i,j) >= 0 && AB(i,j) <= rv * 100 * 100);
  AB.ensures(AB(i,j) >= 0 && AB(i,j) <= sum_size * 100 * 100);

  // Do the part that makes it a 'general' matrix multiply.
  result_(i, j) = (a_ * AB(i, j) + b_ * C_(i, j));
  result_.ensures(result_(i, j) >= 0 && result_(i, j) <= sum_size * 100 * 100 * a_ + b_ * 100);
  A_.dim(0).set_bounds(0, num_rows);
  A_.dim(1).set_bounds(0, sum_size);
  A_.dim(1).set_stride(num_rows);
  
  B_.dim(0).set_bounds(0, sum_size);
  B_.dim(1).set_bounds(0, num_cols);
  B_.dim(1).set_stride(sum_size);

  C_.dim(0).set_bounds(0, num_rows);
  C_.dim(1).set_bounds(0, num_cols);
  C_.dim(1).set_stride(num_rows);
  
  result_.output_buffer().dim(0).set_bounds(0, num_rows);
  result_.output_buffer().dim(1).set_bounds(0, num_cols);
  result_.output_buffer().dim(1).set_stride(num_rows);

  
  if(schedule == 0){

  } else if(schedule == 1) {
    result_
      .tile(i, j, ti[1], tj[1], i, j, 2 * s, 2 * s, TailStrategy::GuardWithIf);
    result_
      .parallel(tj[1])
      .parallel(ti[1])
      ;

    As.compute_root()
        .split(j, jo, ji, s)
        .reorder(i, ji, io, jo)
        .split(jo, jo, jii , 4)
        .parallel(jo)
        ;

  } else if(schedule == 2) {
    result_.tile(i, j, ti[1], tj[1], i, j, 2 * s, 2 * s, TailStrategy::GuardWithIf);
    result_
        .tile(i, j, ii, ji, s, 4)
        .tile(i, j, ti[0], tj[0], i, j, 1, s / 4);
    result_
      // .fuse(tj[1], ti[1], t)
      // .parallel(t)
      .parallel(tj[1])
      .parallel(ti[1])
      ;


    result_.rename(tj[0], t);
    result_.bound(i, 0, num_rows).bound(j, 0, num_cols);

    As.compute_root()
        .split(j, jo, ji, s)
        .reorder(i, ji, io, jo)
        .unroll(i)
        // .vectorize(ji)
        .split(jo, jo, jii , 4)
        .parallel(jo)
        ;

    Atmp.compute_at(As, io)
        // .vectorize(i)
        .unroll(j)
        ;

    AB.compute_at(result_, i)
        .bound_extent(j, 4)
        .unroll(j)
        .bound_extent(i, s)
        // .vectorize(i)
        // .update()
        // .reorder(i, j, rv)
        // .unroll(j)
        // .unroll(rv, 2)
        // .vectorize(i)
        ;
  } else if(schedule == 3) {
    result_.tile(i, j, ti[1], tj[1], i, j, 2 * s, 2 * s, TailStrategy::GuardWithIf);
    result_
        .tile(i, j, ii, ji, s, 4)
        .tile(i, j, ti[0], tj[0], i, j, 1, s / 4);
    result_
      .fuse(tj[1], ti[1], t)
      .parallel(t);


    result_.rename(tj[0], t);

    result_.bound(i, 0, num_rows).bound(j, 0, num_cols);

    As.compute_root()
        .split(j, jo, ji, s)
        .reorder(i, ji, io, jo)
        .unroll(i)
        // .vectorize(ji)
        .split(jo, jo, jii , 4)
        .parallel(jo)
        ;

    Atmp.compute_at(As, io)
        // .vectorize(i)
        .unroll(j)
        ;

    AB.compute_at(result_, i)
        .bound_extent(j, 4)
        .unroll(j)
        .bound_extent(i, s)
        // .vectorize(i)
        .update()
        .reorder(i, j, rv)
        .unroll(j)
        .unroll(rv, 2)
        // .vectorize(i)
        ;
  }

  Target target = Target();
  Target new_target = target
    .with_feature(Target::NoAsserts)
    .with_feature(Target::NoBoundsQuery)
    ;

  std::string name = "gemm";

  if(front) {
      result_.translate_to_pvl(name + "_front_pvl.pvl", {}, {}); 
  } else {
      result_.compile_to_pvl(name + "_pvl-" + std::to_string(schedule) + ".pvl" , {A_, B_, C_}, {}, name, new_target);
  }
}