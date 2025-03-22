# Copyright (c) 2023 Hartmut Kaiser
# Copyright (c) 2025 Shreyas Atre
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

set(CHAPEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/chapel")
set(CHAPEL_PATCH "${CMAKE_CURRENT_SOURCE_DIR}/chapel-2.3.0-frontend.patch")
set(CHAPEL_REQUIRED_COMMIT "3a18b7be6f70ff1dff29c70c73a3ecc84165efdc")  # <-- Replace with your commit SHA

# ---- Ensure submodule is initialized ----
if(NOT EXISTS "${CHAPEL_PATH}/.git")
    message(STATUS "Initializing Chapel submodule...")
    execute_process(
        COMMAND git submodule update --init --recursive extern/chapel
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE GIT_SUBMODULE_RESULT
    )
    if(NOT GIT_SUBMODULE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to initialize Chapel submodule.")
    endif()
endif()

# ---- Get current commit SHA of chapel ----
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY "${CHAPEL_PATH}"
    OUTPUT_VARIABLE CHAPEL_CURRENT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# ---- Check for commit mismatch ----
if(NOT CHAPEL_CURRENT_COMMIT STREQUAL CHAPEL_REQUIRED_COMMIT)
    message(FATAL_ERROR
        "Chapel submodule is not at the required commit.\n"
        "Expected: ${CHAPEL_REQUIRED_COMMIT}\n"
        "Found:    ${CHAPEL_CURRENT_COMMIT}\n"
        "Please checkout the correct commit in extern/chapel."
    )
endif()

# ---- Check if patch is already applied ----
execute_process(
    COMMAND git apply --reverse --check "${CHAPEL_PATCH}"
    WORKING_DIRECTORY "${CHAPEL_PATH}"
    RESULT_VARIABLE PATCH_REVERSED_OK
    OUTPUT_QUIET
    ERROR_QUIET
)

if(PATCH_REVERSED_OK EQUAL 0)
    message(STATUS "Patch already applied. Skipping.")
else()
    message(STATUS "Applying chapel-frontend.patch to Chapel...")

    execute_process(
        COMMAND git apply "${CHAPEL_PATCH}"
        WORKING_DIRECTORY "${CHAPEL_PATH}"
        RESULT_VARIABLE PATCH_RESULT
        OUTPUT_VARIABLE PATCH_OUTPUT
        ERROR_VARIABLE PATCH_ERROR
    )
    if(NOT PATCH_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to apply chapel-frontend.patch:\n${PATCH_ERROR}")
    endif()
endif()

# ---- Add frontend directory ----
add_subdirectory("${CHAPEL_PATH}/frontend")
add_subdirectory(backend)
add_subdirectory(library)

