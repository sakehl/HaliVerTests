cmake_minimum_required(VERSION 3.16)
project(HaliVerExperiments)

set(CMAKE_CXX_STANDARD 11)  # or newer
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_PREFIX_PATH ../../Halide-install)

find_package(Halide REQUIRED)

set(OUTFILE halide.out)

add_executable(blur blur/blur.cpp)
target_link_libraries(blur PRIVATE Halide::Halide)

add_custom_command(
  OUTPUT blur_pvl.pvl
  COMMAND HL_DEBUG_CODEGEN=0 ./blur > ${OUTFILE} 2>&1
  COMMENT "See ${OUTFILE}!"
  DEPENDS blur
  VERBATIM
)

add_custom_target(
    blur_pvl ALL
    DEPENDS blur_pvl.pvl
)

add_executable(hist hist/hist.cpp)
target_link_libraries(hist PRIVATE Halide::Halide)

add_custom_command(
  OUTPUT hist_pvl.pvl
  COMMAND HL_DEBUG_CODEGEN=0 ./hist > ${OUTFILE} 2>&1
  COMMENT "See ${OUTFILE}!"
  DEPENDS hist
  VERBATIM
)

add_custom_target(
    hist_pvl ALL
    DEPENDS hist_pvl.pvl
)

add_executable(conv_layer conv_layer/conv_layer.cpp)
target_link_libraries(conv_layer PRIVATE Halide::Halide)

add_custom_command(
  OUTPUT conv_layer_pvl.pvl
  COMMAND HL_DEBUG_CODEGEN=1 ./conv_layer > ${OUTFILE} 2>&1
  COMMENT "See ${OUTFILE}!"
  DEPENDS conv_layer
  VERBATIM
)

add_custom_target(
    conv_layer_pvl ALL
    DEPENDS conv_layer_pvl.pvl
)

add_executable(gemm gemm/gemm.cpp)
target_link_libraries(gemm PRIVATE Halide::Halide)

add_custom_command(
  OUTPUT gemm_pvl.pvl
  COMMAND HL_DEBUG_CODEGEN=0 ./gemm > ${OUTFILE} 2>&1
  COMMENT "See ${OUTFILE}!"
  DEPENDS gemm
  VERBATIM
)

add_custom_target(
    gemm_pvl ALL
    DEPENDS gemm_pvl.pvl
)

# add_custom_target(
#     blur_front_pvl ALL
#     DEPENDS blur_front_pvl.pvl
# )

# add_executable(bgu bgu.cpp)
# target_link_libraries(bgu PRIVATE Halide::Halide)

# add_custom_command(
#   OUTPUT bgu_pvl.pvl bgu_front_pvl.pvl ${OUTFILE}
#   COMMAND HL_DEBUG_CODEGEN=1 ./bgu > ${OUTFILE} 2>&1
#   COMMENT "See ${OUTFILE}!"
#   DEPENDS bgu
#   VERBATIM
# )

# add_custom_target(
#     bgu_pvl ALL
#     DEPENDS bgu_pvl.pvl
# )

# add_custom_target(
#     bgu_front_pvl ALL
#     DEPENDS bgu_front_pvl.pvl
# )