# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CHPLX_LibraryAddExample)

add_chplx_pseudo_target(examples.librarydir)
add_chplx_pseudo_target_folder(
  examples TARGET examples.librarydir FOLDER "Library/Examples/Targets"
)
add_chplx_pseudo_dependencies(examples examples.librarydir)

set(examples print_range)

foreach(example ${examples})
  chplx_library_add_example(${example})
endforeach()
