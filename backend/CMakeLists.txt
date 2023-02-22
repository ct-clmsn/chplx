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

set(CMAKE_CXX_STANDARD 17)

project(chplx VERSION ${CHPL_VERSION})

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)
find_package(HPX REQUIRED CONFIG)

link_directories(${LD_LIBRARY_PATH})

add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(chplx src/visitor.cpp src/driver.cpp)

target_include_directories(chplx PUBLIC
                           ${LLVM_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(chplx PUBLIC -L. -L${CMAKE_CURRENT_SOURCE_DIR}/../frontend/build/lib -lChplFrontend ${llvm_libs})
