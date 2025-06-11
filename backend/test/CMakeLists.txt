# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CHPLX_Compile)

find_package(ClangFormat)

add_chplx_pseudo_target(tests.unit.backend_dir)
add_chplx_pseudo_target_folder(
  backend
  TARGET tests.unit.backend_dir
  FOLDER "Backend/Tests/Targets"
)

add_chplx_pseudo_dependencies(tests.unit tests.unit.backend_dir)

set(tests
    arr
    cfg
    cond
    expr
    fn
    fnc
    forall
    tup
)

foreach(test ${tests})
  chplx_compile(${test} SOURCE ${test}.chpl)

  if(CLANGFORMAT_FOUND)
    chplx_format_file(
      ${test}
      SOURCES ${test}.cpp ${test}.hpp ${test}_driver.cpp ${test}_driver.hpp
    )
  endif()

  chplx_compare_result(
    ${test}
    SOURCES ${test}.cpp ${test}.hpp ${test}_driver.cpp ${test}_driver.hpp
            "CMakeLists.txt"
    TEST_NAME "tests.unit.backend_dir.${test}"
  )

  # now build the created files
  chplx_compile_project(
    ${test}
    TARGET ${test}
    TEST_NAME "tests.unit.backend_dir.${test}_cpp"
  )

  add_chplx_pseudo_dependencies(tests.unit.backend_dir "${test}_compile_cpp")

endforeach()
