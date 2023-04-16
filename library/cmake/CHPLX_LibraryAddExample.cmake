# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# define a function to simplify adding examples
function(chplx_library_add_example target)
  add_executable(${target} EXCLUDE_FROM_ALL "${target}.cpp")
  target_link_libraries(${target} library)
  add_chplx_pseudo_dependencies(${target} library)
  set_target_properties(${target} PROPERTIES FOLDER "Library/Examples")

  add_chplx_pseudo_dependencies(examples.librarydir ${target})
endfunction()

