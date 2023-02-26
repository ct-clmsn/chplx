# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_custom_target(tests)

set(CHPLX_TEST_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR})

# define a function to simplify adding tests
function(chplx_unit_test target)
  add_executable(${target} EXCLUDE_FROM_ALL "${target}.cpp")
  target_link_libraries(${target} chplx_library)
  add_dependencies(${target} chplx_library)

  add_test(NAME ${target} COMMAND ${target})
  add_dependencies(tests ${target})
endfunction()

add_subdirectory(support)
