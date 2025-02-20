# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.19)

set(CHPLX_MAJOR_VERSION 0)
set(CHPLX_MINOR_VERSION 1)
set(CHPLX_PATCH_VERSION 0)
set(CHPLX_BUILD_VERSION 0)

set(CHPLX_DEVELOPER ON)
set(CHPLX_OFFICIAL_RELEASE 0)

set(CHPLX_VERSION
    ${CHPLX_MAJOR_VERSION}.${CHPLX_MINOR_VERSION}.${CHPLX_PATCH_VERSION}.${CHPLX_BUILD_VERSION}
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE
        STRING
        "Configuration type (one of Debug, RelWithDebInfo, Release, MinSizeRel)"
        FORCE
  )
endif()

project(chplx VERSION ${CHPLX_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
include(CHPLX_Utils)

# ##############################################################################
# build options
if(WIN32)
  set(CHPLX_WITH_PSEUDO_DEPENDENCIES_DEFAULT OFF)
else()
  set(CHPLX_WITH_PSEUDO_DEPENDENCIES_DEFAULT ON)
endif()

chplx_option(
  CHPLX_WITH_EXAMPLES BOOL "Enable building examples (default: ON)" ON ADVANCED
)

chplx_option(
  CHPLX_WITH_TESTS BOOL "Enable building tests (default: ON)" ON ADVANCED
)

chplx_option(
  CHPLX_WITH_PSEUDO_DEPENDENCIES
  BOOL
  "Enable build pseudo target (default: ${CHPLX_WITH_PSEUDO_DEPENDENCIES_DEFAULT})"
  ${CHPLX_WITH_PSEUDO_DEPENDENCIES_DEFAULT}
  ADVANCED
)

chplx_option(
  CHPLX_WITH_PARALLEL_TESTS_BIND_NONE
  BOOL
  "Pass --hpx:bind=none to tests that may run in parallel (cmake -j flag) (default: OFF)"
  OFF
  CATEGORY "Debugging"
  ADVANCED
)

set(
  CHPLX_WITH_FETCH_FMT
  ON
  CACHE
  BOOL
  "Fetch FMT"
)

set(
  CHPLX_WITH_FETCH_HPX
  ON
  CACHE
  BOOL
  "Fetch HPX"
)

add_chplx_pseudo_target(tests)
add_chplx_pseudo_target(tests.unit)
add_chplx_pseudo_dependencies(tests tests.unit)

if(CHPLX_WITH_TESTS)
  enable_testing()
  include(CTest)
endif()

add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(library)

