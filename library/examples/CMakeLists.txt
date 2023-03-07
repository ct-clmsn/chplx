# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_chplx_pseudo_target(examples)

set(CHPLX_TEST_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR})

# define a function to simplify adding tests
function(chplx_example target)
  add_executable(${target} EXCLUDE_FROM_ALL "${target}.cpp")
  target_link_libraries(${target} chplx_library)
  set_target_properties(${target} PROPERTIES FOLDER "Examples")
  add_chplx_pseudo_dependencies(${target} chplx_library)
  add_chplx_pseudo_dependencies(examples ${target})
endfunction()

set(examples print_range)

foreach(example ${examples})
  chplx_example(${example})
endforeach()
