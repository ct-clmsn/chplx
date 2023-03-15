# Copyright (c) 2023 Hartmut Kaiser
# Copyright (c) 2023 Christopher Taylor
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
# 
cmake_minimum_required(VERSION 3.25)

set(CHPL_MAJOR_VERSION 1)
set(CHPL_MINOR_VERSION 30)
set(CHPL_PATCH_VERSION 0)
set(CHPL_BUILD_VERSION 0)

set(CHPL_DEVELOPER ON)
set(CHPL_OFFICIAL_RELEASE 0)

set(CHPL_VERSION ${CHPL_MAJOR_VERSION}.${CHPL_MINOR_VERSION}.${CHPL_PATCH_VERSION})
set(CHPL_VERSION ${CHPL_MAJOR_VERSION}.${CHPL_MINOR_VERSION}.${CHPL_PATCH_VERSION}.${CHPL_BUILD_VERSION})

if(APPLE)
set(CMAKE_CXX_STANDARD 17)
else()
set(CMAKE_CXX_STANDARD 20)
endif()

project(chplx VERSION ${CHPL_VERSION})

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)

link_directories(${LD_LIBRARY_PATH})

add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_compile_definitions($<$<BOOL:${MSVC}>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>)

set(chplx_sources
   src/symboltypes.cpp
   src/symbolbuildingvisitor.cpp
   src/programtree.cpp
   src/programtreebuildingvisitor.cpp
   src/codegenvisitor.cpp
   src/driver.cpp
)

set(chplx_headers
   include/hpx/symboltypes.hpp
   include/hpx/symbolbuildingvisitor.hpp
   include/hpx/programtree.hpp
   include/hpx/programtreebuildingvisitor.hpp
   include/hpx/codegenvisitor.hpp
   include/ErrorGuard.h
)

if(MSVC)
  set(chplx_sources ${chplx_sources} src/getopt.c)
  set(chplx_headers ${chplx_headers} include/getopt.h)
endif()

add_executable(chplx ${chplx_sources} ${chplx_headers})

target_include_directories(chplx PUBLIC
                           ${LLVM_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_directories(chplx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/build/lib)
target_link_libraries(chplx PUBLIC ChplFrontend ${llvm_libs})
target_link_libraries(chplx PRIVATE fmt::fmt-header-only)
