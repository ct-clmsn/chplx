# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_chplx_pseudo_target(tests.unit.library_dir)
add_chplx_pseudo_target_folder(
  library TARGET tests.unit.library_dir FOLDER "Library/Tests/Targets"
)
add_chplx_pseudo_dependencies(tests.unit tests.unit.library_dir)

add_subdirectory(unit)
