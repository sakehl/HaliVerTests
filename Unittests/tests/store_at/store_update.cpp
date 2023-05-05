#include "Halide.h"
#include <stdio.h>
#include <vector>

using namespace Halide;

int main(int argc, char *argv[]) {
  Func f("f"), out("out");
  Var x("x"), y("y");
  f(x, y) = x + y;
  f.ensures(f(x,y) == x+y);
  f(x, x) = 2*f(x,x);
  f.ensures(implies(y == x, f(x,y) == 2*(x+y)));
  f.ensures(implies(y != x, f(x,y) == x+y));

  out(x, y) = f(x,y) + 1;
  out.ensures(implies(y == x, out(x,y) == 2*(x+y)+1));
  out.ensures(implies(y != x, out(x,y) == x+y+1));

  f.store_at(out, y);
  f.compute_at(out, x);

  Var xo("xo"), xi("xi");

  int nx = 100, ny = 42;

  out.output_buffer().dim(0).set_bounds(0,nx);
  out.output_buffer().dim(1).set_bounds(0,ny);
  out.output_buffer().dim(1).set_stride(nx);
  
  Target target = Target();
  Target new_target = target
    .with_feature(Target::NoAsserts)
    .with_feature(Target::NoBoundsQuery)
    ;
  
  std::vector<Annotation> pipeline_anns;

  std::string name = argv[1];
  out.compile_to_pvl(name + "_back.pvl" , {}, pipeline_anns, name, new_target);
}