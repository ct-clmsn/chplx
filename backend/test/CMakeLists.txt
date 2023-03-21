# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CHPLX_Compile)

find_package(ClangFormat)

add_chplx_pseudo_target(tests.unit.backend)
add_chplx_pseudo_dependencies(tests.unit tests.unit.backend)

set(tests arr fn)

foreach(test ${tests})
  chplx_compile(${test} SOURCE ${test}.chpl)
  add_chplx_pseudo_dependencies(tests.unit.backend ${test}_test)

  if(CLANGFORMAT_FOUND)
    chplx_format_file(${test} SOURCES ${test}.cpp)
  endif()

  chplx_compare_result(${test} SOURCE ${test}.cpp GOOD ${test}.good)

endforeach()
