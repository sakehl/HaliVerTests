#include "Halide.h"
#include <stdio.h>
#include <vector>

using namespace Halide;

int main(int argc, char *argv[]) {

  Func f("f"), out("out");
  Var x("x"), y("y");

  out(x, y) = x + y;
  out.ensures(out(x,y) == x + y);

  out.output_buffer().dim(1).set_stride(out.output_buffer().dim(0).extent());
  out.output_buffer().dim(1).set_min(0);
  out.output_buffer().dim(0).set_min(0);

  Target target = Target();
  Target new_target = target
    .with_feature(Target::NoAsserts)
    .with_feature(Target::NoBoundsQuery)
    ;
  
  std::vector<Annotation> pipeline_anns;

  std::string name = argv[1];
  out.translate_to_pvl(name +"_front.pvl", {}, pipeline_anns); 
  out.compile_to_pvl(name + "_back.pvl" , {}, pipeline_anns, name, new_target);
}