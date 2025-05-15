# Copyright (c) 2023 Hartmut Kaiser
# Copyright (c) 2023 Christopher Taylor
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.19)

set(CHPLX_BACKEND_MAJOR_VERSION 1)
set(CHPLX_BACKEND_MINOR_VERSION 30)
set(CHPLX_BACKEND_PATCH_VERSION 0)
set(CHPLX_BACKEND_BUILD_VERSION 0)

set(CHPLX_BACKEND_DEVELOPER ON)
set(CHPLX_BACKEND_OFFICIAL_RELEASE 0)

set(CHPLX_BACKEND_VERSION
    ${CHPLX_BACKEND_MAJOR_VERSION}.${CHPLX_BACKEND_MINOR_VERSION}.${CHPLX_BACKEND_PATCH_VERSION}.${CHPLX_BACKEND_BUILD_VERSION}
)

project(chplx_backend VERSION ${CHPLX_BACKEND_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

if(CHPLX_WITH_FETCH_FMT)
  include(FetchContent)
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
    GIT_PROGRESS TRUE
  )
  FetchContent_MakeAvailable(fmt)
else()
  find_package(fmt REQUIRED CONFIG)
endif()

link_directories(${LD_LIBRARY_PATH})

add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

set(chplx_sources
    src/symboltypes.cpp src/symbolbuildingvisitor.cpp src/programtree.cpp
    src/programtreebuildingvisitor.cpp src/codegenvisitor.cpp src/cmakegen.cpp src/driver.cpp
    src/utils.cpp
)

set(chplx_headers
    include/hpx/symboltypes.hpp include/hpx/symbolbuildingvisitor.hpp
    include/hpx/programtree.hpp include/hpx/programtreebuildingvisitor.hpp
    include/hpx/codegenvisitor.hpp include/hpx/cmakegen.hpp include/hpx/utils.hpp
    include/ErrorGuard.h
)

if(MSVC)
  set(chplx_sources ${chplx_sources} src/getopt.c)
  set(chplx_headers ${chplx_headers} include/getopt.h)
endif()

add_executable(chplx ${chplx_sources} ${chplx_headers})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(chplx PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -Wall)
    target_link_options(chplx PRIVATE -fsanitize=address,undefined)
endif()

target_include_directories(
  chplx
  PUBLIC ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_directories(
  chplx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../build/frontend/lib
)
target_link_libraries(chplx PRIVATE ChplFrontend ${llvm_libs})
target_link_libraries(chplx PRIVATE fmt::fmt-header-only)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # GCC often puts <filesystem> in libstdc++fs
  target_link_libraries(chplx PRIVATE stdc++fs stdc++)
else()
  # Clang + libc++ may require both the fs library and the ABI
  target_link_libraries(chplx PRIVATE c++fs c++abi)
endif()

set_target_properties(chplx PROPERTIES FOLDER "Backend")
if(MSVC)
  # disable consteval in {fmt} on MSVC
  target_compile_definitions(chplx PUBLIC FMT_CONSTEVAL=)
endif()

set_target_properties(acc_gen PROPERTIES FOLDER "Backend/Support")
set_target_properties(clang-tablegen-targets PROPERTIES FOLDER "Backend/Support")
set_target_properties(intrinsics_gen PROPERTIES FOLDER "Backend/Support")
set_target_properties(omp_gen PROPERTIES FOLDER "Backend/Support")
if(TARGET RISCVTargetParserTableGen)
  set_target_properties(RISCVTargetParserTableGen PROPERTIES FOLDER "Backend/Support")
endif()

if(CHPLX_WITH_TESTS)
  enable_testing()
  include(CTest)
  add_subdirectory(test)
endif()

install(TARGETS chplx
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin
)
