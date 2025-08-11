# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.19)

set(CHPLX_LIBRARY_MAJOR_VERSION 0)
set(CHPLX_LIBRARY_MINOR_VERSION 1)
set(CHPLX_LIBRARY_PATCH_VERSION 0)
set(CHPLX_LIBRARY_BUILD_VERSION 0)

set(CHPLX_LIBRARY_DEVELOPER ON)
set(CHPLX_LIBRARY_OFFICIAL_RELEASE 0)

set(CHPLX_LIBRARY_VERSION
    ${CHPLX_LIBRARY_MAJOR_VERSION}.${CHPLX_LIBRARY_MINOR_VERSION}.${CHPLX_LIBRARY_PATCH_VERSION}.${CHPLX_LIBRARY_BUILD_VERSION}
)

set(CMAKE_CXX_STANDARD 20)
set(CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if(NOT WIN32
   AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
   AND NOT APPLE
)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ -Ofast")
endif()

project(chplx_library VERSION ${CHPLX_LIBRARY_VERSION})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# global dependencies
find_package(fmt REQUIRED CONFIG)
find_package(HPX REQUIRED CONFIG)

# create library target
set(chplx_library_detail_headers
    include/chplx/detail/generate_annotation.hpp
    include/chplx/detail/iterator_generator.hpp
)
source_group("Header Files/chplx/detail" FILES ${chplx_library_detail_headers})

set(chplx_library_domain_headers include/chplx/domains/base_rectangular.hpp
                                 include/chplx/domains/default_rectangular.hpp
)
source_group("Header Files/chplx/domains" FILES ${chplx_library_domain_headers})

set(chplx_library_headers
    include/chplx/adapt_array.hpp
    include/chplx/adapt_domain.hpp
    include/chplx/adapt_range.hpp
    include/chplx/adapt_tuple.hpp
    include/chplx/array.hpp
    include/chplx/assoc_domain.hpp
    include/chplx/atomic.hpp
    include/chplx/begin.hpp
    include/chplx/cobegin.hpp
    include/chplx/coforall_loop.hpp
    include/chplx/dmap.hpp
    include/chplx/domain.hpp
    include/chplx/for_loop.hpp
    include/chplx/forall_loop.hpp
    include/chplx/locale.hpp
    include/chplx/nothing.hpp
    include/chplx/range.hpp
    include/chplx/single_variable.hpp
    include/chplx/string.hpp
    include/chplx/sync_variable.hpp
    include/chplx/tuple.hpp
    include/chplx/types.hpp
    include/chplx/util.hpp
    include/chplx/version.hpp
    include/chplx/write.hpp
    include/chplx/zip.hpp
    include/chplx/setenv.h
)
source_group("Header Files/chplx" FILES ${chplx_library_headers})

set(chplx_library_detail_sources
    src/detail/generate_annotation.cpp
)
source_group("Source Files/detail" FILES ${chplx_library_detail_sources})

set(chplx_library_domain_sources src/domains/dmap.cpp)
source_group("Source Files/domains" FILES ${chplx_library_domain_sources})

set(chplx_library_sources ${chplx_library_domain_sources} src/locale.cpp
                          src/version.cpp
)

set(chplx_library_headers
    ${chplx_library_detail_headers} ${chplx_library_domain_headers}
    ${chplx_library_headers} include/chplx.hpp
)

if(MSVC)
  set(chplx_library_sources ${chplx_library_sources} src/setenv.c)
endif()

add_library(library STATIC
    ${chplx_library_sources}
    ${chplx_library_detail_sources}
    ${chplx_library_headers}
)

if(NOT WIN32
   AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
   AND APPLE
)
  find_library(CORE_LIBPATH NAMES CoreFoundation REQUIRED)

  target_compile_options(library PUBLIC -nostdlib -std=c++20 -stdlib=libc++)

  target_link_libraries(library PRIVATE ${CORE_LIBPATH})
  target_link_libraries(library PUBLIC c++)
endif()

target_link_libraries(library PUBLIC fmt::fmt-header-only HPX::hpx)

target_include_directories(
  library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                 ${CMAKE_CURRENT_BINARY_DIR}/include
)

# define additional properties
define_property(
  TARGET
  PROPERTY TARGET_BINARY_DIR
  FULL_DOCS "Base binary directory"
  BRIEF_DOCS "Base binary directory"
)

set_target_properties(
  library
  PROPERTIES OUTPUT_NAME chplx_library
             FOLDER "Library"
             TARGET_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}"
)

# generate version file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/chplx/config/version.hpp" @ONLY
)

find_package(PythonInterp)

if(NOT PYTHONINTERP_FOUND)
  chplx_warn(
    "A python interpreter could not be found. The test suite can not be run automatically."
  )
endif()

# Configure chplxrun.py
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/templates/chplxrun.py.in"
  "${PROJECT_BINARY_DIR}/bin/chplxrun.py" @ONLY
)

# generate cmake packages
include(CHPLX_GeneratePackage)

if(CHPLX_WITH_TESTS)
  enable_testing()
  include(CTest)
  add_subdirectory(test)
endif()

if(CHPLX_WITH_EXAMPLES)
  add_chplx_pseudo_target(examples)
  add_subdirectory(examples)
endif()
