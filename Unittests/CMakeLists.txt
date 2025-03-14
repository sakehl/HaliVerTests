cmake_minimum_required(VERSION 3.16)
project(HaliVerExperiments)

include(CTest)

set(CMAKE_CXX_STANDARD 11)  # or newer
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_PREFIX_PATH ../../halide/install)

find_package(Halide REQUIRED)

set(OUTFILE halide.out)
set(VERCORS /home/lars/data/vercors/bin/vct)


function(build_unit_test)
  set(options SHOULDFAIL AND_FRONT)
  set(oneValueArgs TARGET DIR)
  set(multiValueArgs LABELS)
  cmake_parse_arguments(UT "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )

  add_executable(${UT_TARGET} tests/${UT_DIR}/${UT_TARGET}.cpp)

  target_link_libraries(${UT_TARGET} PRIVATE Halide::Halide)

  if(${UT_AND_FRONT})
    add_custom_command(
      OUTPUT ${UT_TARGET}_front.pvl ${UT_TARGET}_mem.c
      COMMAND ./${UT_TARGET} ${UT_TARGET} -mem_only
      DEPENDS ${UT_TARGET}
      VERBATIM
    )

    add_custom_target(
      ${UT_TARGET}_pvl ALL
      DEPENDS ${UT_TARGET}_front.pvl ${UT_TARGET}_mem.c
    )

    add_test(NAME ${UT_TARGET}_front.pvl
      COMMAND ${VERCORS} --silicon-quiet ${UT_TARGET}_front.pvl
      )
    set_tests_properties(${UT_TARGET}_front.pvl PROPERTIES
      WILL_FAIL ${UT_SHOULDFAIL}
      LABELS ${UT_DIR}:front:${UT_LABELS}
    )
  else()
    add_custom_command(
      OUTPUT ${UT_TARGET}_mem.c
      COMMAND ./${UT_TARGET} ${UT_TARGET} -mem_only
      DEPENDS ${UT_TARGET}
      VERBATIM
    )

    add_custom_target(
      ${UT_TARGET}_pvl ALL
      DEPENDS ${UT_TARGET}_mem.c
    )
  endif()

  add_test(NAME ${UT_TARGET}_mem.c
    COMMAND ${VERCORS} --silicon-quiet ${UT_TARGET}_mem.c
  )
  set_tests_properties(${UT_TARGET}_mem.c
    PROPERTIES
    WILL_FAIL ${UT_SHOULDFAIL}
    LABELS ${UT_DIR}:back:${UT_LABELS}
  )
endfunction()

# Simple algorithms
build_unit_test(TARGET pure_func DIR alg AND_FRONT)
build_unit_test(TARGET update DIR alg AND_FRONT)
build_unit_test(TARGET update_full DIR alg AND_FRONT)
build_unit_test(TARGET update_full_fail DIR alg SHOULDFAIL AND_FRONT)
build_unit_test(TARGET reduction DIR alg AND_FRONT)
build_unit_test(TARGET reduction_multi DIR alg AND_FRONT)
build_unit_test(TARGET reduction_multi_fail DIR alg SHOULDFAIL AND_FRONT)
build_unit_test(TARGET input_buffer DIR alg AND_FRONT)

build_unit_test(TARGET tuple DIR alg)
build_unit_test(TARGET tuple_pipeline DIR alg)
build_unit_test(TARGET multidim DIR alg)
build_unit_test(TARGET reduction_where DIR alg)

# Floats / Rationals
build_unit_test(TARGET input_floats DIR alg AND_FRONT)

# Parallel schedules
build_unit_test(TARGET par_par DIR parallel)
build_unit_test(TARGET par_red DIR parallel)
build_unit_test(TARGET par_seq DIR parallel)
build_unit_test(TARGET seq_par DIR parallel)

# Unroll schedules
build_unit_test(TARGET unroll_outer DIR unroll)
build_unit_test(TARGET unroll_par DIR unroll)
build_unit_test(TARGET unroll_red DIR unroll)
build_unit_test(TARGET unroll_ser DIR unroll)

# Split schedules
build_unit_test(TARGET split_exact DIR split)
build_unit_test(TARGET split_if DIR split)
build_unit_test(TARGET split_round DIR split)
# build_unit_test(TARGET split_shift DIR split)

# Fuse schedules
build_unit_test(TARGET fuse_par DIR fuse)
build_unit_test(TARGET fuse_ser DIR fuse)

# Compute_at
build_unit_test(TARGET inline DIR compute_at)
build_unit_test(TARGET compute_half DIR compute_at)
build_unit_test(TARGET compute_root DIR compute_at)
build_unit_test(TARGET compute_update DIR compute_at)

# Store_at
build_unit_test(TARGET store_half DIR store_at)
build_unit_test(TARGET store_root DIR store_at)
build_unit_test(TARGET store_update DIR store_at)

# Reorder
build_unit_test(TARGET reorder_par_red DIR reorder)
build_unit_test(TARGET reorder_par_red_2 DIR reorder)
build_unit_test(TARGET reorder_par_red_3 DIR reorder)
build_unit_test(TARGET reorder_par_ser DIR reorder)
build_unit_test(TARGET reorder_seq_par DIR reorder)
build_unit_test(TARGET reorder_seq_red DIR reorder)
build_unit_test(TARGET reorder DIR reorder)

# Known limitations
build_unit_test(TARGET pure_func_no_bounds DIR limitations)