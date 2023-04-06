cmake_minimum_required(VERSION 3.16)
project(HalideDiffusion)

set(CMAKE_CXX_STANDARD 11)  # or newer
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_PREFIX_PATH ../../Halide-install)


find_package(Halide REQUIRED)
find_package(Threads REQUIRED)

add_executable(even even.cpp)
target_link_libraries(even PRIVATE Halide::Halide)

set(OUTFILE halide.out)

add_custom_command(
  OUTPUT even_pvl.pvl ${OUTFILE}
  COMMAND HL_DEBUG_CODEGEN=0 ./even --pvl 
  COMMENT "See ${OUTFILE}!"
  DEPENDS even
  VERBATIM
)

add_custom_target(
    even_pvl.pvl_t ALL
    DEPENDS even_pvl.pvl
)