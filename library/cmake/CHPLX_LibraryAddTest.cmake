# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# define a function to simplify adding tests
function(chplx_library_add_unit_test target)
  add_executable(${target}_test EXCLUDE_FROM_ALL "${target}.cpp")
  target_link_libraries(${target}_test library HPX::wrap_main)
  add_chplx_pseudo_dependencies(${target}_test library)
  set_target_properties(${target}_test PROPERTIES FOLDER "Library/Tests")

  add_chplx_unit_test("library_dir" ${target} ${ARGV} TIMEOUT 60)
  if(TARGET ${target})
    set_target_properties(${target} PROPERTIES FOLDER "Library/Tests/Targets")
  endif()
endfunction()

