# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.25)

set(CHPLX_MAJOR_VERSION 0)
set(CHPLX_MINOR_VERSION 1)
set(CHPLX_PATCH_VERSION 0)
set(CHPLX_BUILD_VERSION 0)

set(CHPLX_DEVELOPER ON)
set(CHPLX_OFFICIAL_RELEASE 0)

set(CHPLX_VERSION ${CHPLX_MAJOR_VERSION}.${CHPLX_MINOR_VERSION}.${CHPLX_PATCH_VERSION}.${CHPLX_BUILD_VERSION})

set(CMAKE_CXX_STANDARD 20)

project(chplx_library VERSION ${CHPLX_VERSION})

find_package(fmt REQUIRED CONFIG)
find_package(HPX REQUIRED CONFIG)

add_compile_definitions($<$<BOOL:${MSVC}>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>)

set(chplx_library_detail_headers
    include/chplx/detail/range_index_generator.hpp)
source_group("Header Files/chplx/detail" FILES
    ${chplx_library_detail_headers})

set(chplx_library_headers
    include/chplx/generate_indicies.hpp
    include/chplx/range.hpp
    include/chplx/version.hpp)
source_group("Header Files/chplx" FILES
    ${chplx_library_headers})


set(chplx_library_sources src/version.cpp)
set(chplx_library_headers
    ${chplx_library_detail_headers}
    ${chplx_library_headers}
    include/chplx.hpp)

add_library(chplx_library STATIC
    ${chplx_library_sources}
    ${chplx_library_headers})

target_link_libraries(chplx_library PUBLIC fmt::fmt-header-only HPX::hpx)

target_include_directories(chplx_library PUBLIC include)
target_include_directories(chplx_library PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

# generate version file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/chplx/config/version.hpp" @ONLY
)

add_subdirectory(test)
