# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_program(
  CLANGFORMAT_EXECUTABLE clang-format
  PATHS ${CMAKE_SYSTEM_PREFIX_PATH} ENV CLANGFORMAT_ROOT
  PATH_SUFFIXES bin dist/bin
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  ClangFormat REQUIRED_VARS CLANGFORMAT_EXECUTABLE
)

if(CLANGFORMAT_EXECUTABLE)
  set(CLANGFORMAT_FOUND TRUE)
endif()
