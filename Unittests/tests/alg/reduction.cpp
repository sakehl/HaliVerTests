#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {

  Func f("f"), out("out");
  Var x("x"), y("y");

  f(x) = 1;
  f.ensures(f(x) == 1);
  f.compute_root();

  RDom r(0,42, "r");
  out(x, y) = x + y;
  out.ensures(out(x,y) == x+y);
  out(x,y) += f(r);
  out.invariant(out(x,y) == x+y+r);
  out.ensures(out(x,y) == x+y+42); 

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
  out.translate_to_pvl(name +"_front.pvl", {}, pipeline_anns); 
  out.compile_to_c(name + ".c" , {}, pipeline_anns, name, new_target);
}