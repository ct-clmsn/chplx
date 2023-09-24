# Copyright 2021-2023 Hewlett Packard Enterprise Development LP
# Other additional copyright holders may be indicated within.
#
# The entirety of this work is licensed under the Apache License,
# Version 2.0 (the "License"); you may not use this file except
# in compliance with the License.
#
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.19)

set(CHPLX_FRONTEND_MAJOR_VERSION 1)
set(CHPLX_FRONTEND_MINOR_VERSION 30)
set(CHPLX_FRONTEND_PATCH_VERSION 0)
set(CHPLX_FRONTEND_BUILD_VERSION 0)

set(CHPLX_FRONTEND_DEVELOPER ON)
set(CHPLX_FRONTEND_OFFICIAL_RELEASE 0)

set(CHPLX_FRONTEND_VERSION
    ${CHPLX_FRONTEND_MAJOR_VERSION}.${CHPLX_FRONTEND_MINOR_VERSION}.${CHPLX_FRONTEND_PATCH_VERSION}.${CHPLX_FRONTEND_BUILD_VERSION}
)

project(ChplFrontend VERSION ${CHPLX_FRONTEND_VERSION})

if(NOT $LLVM_ROOT)
  find_package(LLVM REQUIRED CONFIG HINTS ${LLVM_ROOT})
else()
  find_package(LLVM REQUIRED CONFIG)
endif()
if(NOT $CLANG_ROOT)
  find_package(Clang REQUIRED CONFIG HINTS ${CLANG_ROOT})
else()
  find_package(Clang REQUIRED CONFIG)
endif()

add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

# CHPL_HOME may have been defined by an upstream CMakeLists.txt, so check if it
# doesn't exist before trying to create it.
if(NOT DEFINED CHPL_HOME)
  message(STATUS "CHPL_HOME not defined, adding to cache...")
  set(CHPL_HOME
      ${CMAKE_PROJECT_SOURCE_DIR}
      CACHE STRING "The CHPL_HOME environment variable for tests to use"
  )
else()
  set(CHPL_HOME ${CHPL_HOME})
endif()

if(NOT DEFINED ${CHPL_HOME})
  set(CHPL_HOME ${ENV${CHPL_HOME}})
endif()

# Options to integrate LLVM support library into dyno TODO: enable easier cmake
# builds of dyno (outside of Chapel's Makefiles) by falling back on
# find_package(LLVM) if DYNO_LLVM_COMP_ARGS / DYNO_LLVM_LINK_ARGS are empty.
if(NOT DEFINED CHPL_HOST_BUNDLED_COMPILE_ARGS)
  set(CHPL_HOST_BUNDLED_COMPILE_ARGS CACHE STRING
                                           "Override LLVM=bundled compile args"
  )
endif()
if(NOT DEFINED CHPL_HOST_SYSTEM_COMPILE_ARGS)
  set(CHPL_HOST_SYSTEM_COMPILE_ARGS CACHE STRING
                                          "Overide LLVM=system compile args"
  )
endif()
if(NOT DEFINED CHPL_HOST_BUNDLED_LINK_ARGS)
  set(CHPL_HOST_BUNDLED_LINK_ARGS CACHE STRING
                                        "Override LLVM=bundled link args"
  )
endif()
if(NOT DEFINED CHPL_HOST_SYSTEM_LINK_ARGS)
  set(CHPL_HOST_SYSTEM_LINK_ARGS CACHE STRING "Override LLVM=system link args")
endif()

if(NOT DEFINED CHPL_LLVM_COMP_ARGS)
  # TODO: See if we can use these directly, I am not sure we can because the
  # CMake way to concat strings requires an output variable
  string(CONCAT CHPL_LLVM_COMP_ARGS ${CHPL_HOST_BUNDLED_COMPILE_ARGS} " "
                ${CHPL_HOST_SYSTEM_COMPILE_ARGS}
  )
  # remove whitespace or you'll get complaints about an error according to
  # policy CMP0004
  if(CHPL_LLVM_COMP_ARGS)
    string(STRIP ${CHPL_LLVM_COMP_ARGS} CHPL_LLVM_COMP_ARGS)
  endif()
endif()
if(NOT DEFINED CHPL_LLVM_LINK_ARGS)
  string(CONCAT CHPL_LLVM_LINK_ARGS ${CHPL_HOST_BUNDLED_LINK_ARGS} " "
                ${CHPL_HOST_SYSTEM_LINK_ARGS}
  )
  string(STRIP ${CHPL_LLVM_LINK_ARGS} CHPL_LLVM_LINK_ARGS)
endif()

# message(DEBUG "CHPL_LLVM_COMP_ARGS: ${CHPL_LLVM_COMP_ARGS}") message(DEBUG
# "CHPL_LLVM_LINK_ARGS: ${CHPL_LLVM_LINK_ARGS}")

# Don't pass through jemalloc for the time being.
string(REPLACE "-ljemalloc" "" CHPL_LLVM_LINK_ARGS "${CHPL_LLVM_LINK_ARGS}")

set(CHPL_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CHPL_MAIN_INCLUDE_DIR ${CHPL_MAIN_SRC_DIR}/include)
set(CHPL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

# save the version
# configure_file(${CHPL_MAIN_INCLUDE_DIR}/include/chpl/config/config.h.cmake
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/chpl/config/config.h.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/include/chpl/config/config.h
)

# request C++17
set(CMAKE_CXX_STANDARD 17)
set(CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# request C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# generate the compile_commands.json compilation database tools like the
# fieldsUsed linter require this
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add an option to enable/disable assertions this is based upon LLVM cmake files
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if(NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  option(DYNO_ENABLE_ASSERTIONS "Enable assertions" OFF)
else()
  option(DYNO_ENABLE_ASSERTIONS "Enable assertions" ON)
endif()

# adjust C/C++ flags to remove -DNDEBUG if assertions is enabled this is based
# upon LLVM cmake files
if(DYNO_ENABLE_ASSERTIONS)
  # MSVC doesn't like _DEBUG on release builds. See PR 4379.
  if(NOT MSVC)
    add_definitions(-D_DEBUG)
  endif()
  # On non-Debug builds cmake automatically defines NDEBUG, so we explicitly
  # undefine it:
  if(NOT uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    # NOTE: use `add_compile_options` rather than `add_definitions` since
    # `add_definitions` does not support generator expressions.
    add_compile_options(
      $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:-UNDEBUG>
    )

    # Also remove /D NDEBUG to avoid MSVC warnings about conflicting defines.
    foreach(
      flags_var_to_scrub
      CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL
    )
      string(REGEX REPLACE "(^| )[/-]D *NDEBUG($| )" " "
                           "${flags_var_to_scrub}" "${${flags_var_to_scrub}}"
      )
    endforeach()
  endif()
endif()

add_compile_definitions($<$<BOOL:${MSVC}>:YY_NO_UNISTD_H>)
add_compile_definitions(
  $<$<BOOL:${MSVC}>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>
)

# set include directories for -I
include_directories(include)

# lib/CMakeLists.txt defines the target ChplFrontend
add_subdirectory(lib)
# also make sure headers are included in the target
add_subdirectory(include/chpl)
# Support for documentation of AST header
add_subdirectory(doc)
# Utils like C++ linters for this codebase
add_subdirectory(util)

# for now, disable tests

# Check if the directory is there, because in release tarballs, it shouldn't be
#if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
#  # Support for C++ compiler unit tests Needs to happen in this file for ctest
#  # to work in this dir
#  enable_testing()
#  add_subdirectory(test)
#endif()
