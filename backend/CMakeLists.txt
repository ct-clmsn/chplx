# Copyright (c) 2023 Hartmut Kaiser
# Copyright (c) 2023 Christopher Taylor
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.19)

set(CHPLX_MAJOR_VERSION 1)
set(CHPLX_MINOR_VERSION 30)
set(CHPLX_PATCH_VERSION 0)
set(CHPLX_BUILD_VERSION 0)

set(CHPLX_DEVELOPER ON)
set(CHPLX_OFFICIAL_RELEASE 0)

set(CHPL_VERSION
    ${CHPLX_MAJOR_VERSION}.${CHPLX_MINOR_VERSION}.${CHPLX_PATCH_VERSION}.${CHPLX_BUILD_VERSION}
)

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

set(chplx_sources
    src/symboltypes.cpp src/symbolbuildingvisitor.cpp src/programtree.cpp
    src/programtreebuildingvisitor.cpp src/codegenvisitor.cpp src/driver.cpp
)

set(chplx_headers
    include/hpx/symboltypes.hpp include/hpx/symbolbuildingvisitor.hpp
    include/hpx/programtree.hpp include/hpx/programtreebuildingvisitor.hpp
    include/hpx/codegenvisitor.hpp include/ErrorGuard.h
)

if(MSVC)
  set(chplx_sources ${chplx_sources} src/getopt.c)
  set(chplx_headers ${chplx_headers} include/getopt.h)
endif()

add_executable(chplx ${chplx_sources} ${chplx_headers})

target_include_directories(
  chplx
  PUBLIC ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(
  chplx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/build/lib
)
target_link_libraries(chplx PUBLIC ChplFrontend ${llvm_libs})
target_link_libraries(chplx PRIVATE fmt::fmt-header-only)

set_target_properties(chplx PROPERTIES FOLDER "Backend")

set_target_properties(acc_gen PROPERTIES FOLDER "Backend/Support")
set_target_properties(clang-tablegen-targets PROPERTIES FOLDER "Backend/Support")
set_target_properties(intrinsics_gen PROPERTIES FOLDER "Backend/Support")
set_target_properties(omp_gen PROPERTIES FOLDER "Backend/Support")
set_target_properties(RISCVTargetParserTableGen PROPERTIES FOLDER "Backend/Support")

if(CHPLX_WITH_TESTS)
  enable_testing()
  include(CTest)
  add_subdirectory(test)
endif()
