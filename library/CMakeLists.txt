# Copyright (c) 2023-2025 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.19)

set(CHPLX_LIBRARY_MAJOR_VERSION 0)
set(CHPLX_LIBRARY_MINOR_VERSION 1)
set(CHPLX_LIBRARY_PATCH_VERSION 0)
set(CHPLX_LIBRARY_BUILD_VERSION 0)
set(CHPL_OFFICIAL_RELEASE false)
set(CHPLX_LIBRARY_DEVELOPER ON)
set(CHPLX_LIBRARY_OFFICIAL_RELEASE 0)

set(CHPLX_LIBRARY_VERSION
    ${CHPLX_LIBRARY_MAJOR_VERSION}.${CHPLX_LIBRARY_MINOR_VERSION}.${CHPLX_LIBRARY_PATCH_VERSION}.${CHPLX_LIBRARY_BUILD_VERSION}
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(chplx_library VERSION ${CHPLX_LIBRARY_VERSION})

include(FetchContent)

set(HPX_USE_CMAKE_CXX_STANDARD
    ON
    CACHE BOOL ""
)
set(HPX_WITH_CXX20
    ON
    CACHE BOOL ""
)

set(hpx_repository
    https://github.com/STEllAR-GROUP/hpx.git
    CACHE STRING "URL for hpx"
)

if(CHPLX_WITH_FETCH_HPX)
  fetchcontent_declare(
    jemalloc
    GIT_REPOSITORY https://github.com/jemalloc/jemalloc.git
    GIT_TAG 5.3.0
  )

  # 2) Populate & build/install via Autotools at configure time
  fetchcontent_getproperties(jemalloc)
  if(NOT jemalloc_POPULATED)
    fetchcontent_populate(jemalloc)

    # where we'll stage the install
    set(JEMALLOC_INSTALL_DIR ${CMAKE_BINARY_DIR}/_deps/jemalloc-install)
    file(MAKE_DIRECTORY ${JEMALLOC_INSTALL_DIR})

    # run autogen/configure/make/install
    execute_process(
      COMMAND ${jemalloc_SOURCE_DIR}/autogen.sh
      WORKING_DIRECTORY ${jemalloc_SOURCE_DIR}
      OUTPUT_QUIET
    )
    execute_process(
      COMMAND ./configure --prefix=${JEMALLOC_INSTALL_DIR} --enable-shared
              --disable-static
      WORKING_DIRECTORY ${jemalloc_SOURCE_DIR}
      OUTPUT_QUIET
    )
    execute_process(
      COMMAND make -j # or make -j${CMAKE_BUILD_PARALLEL_LEVEL}
      WORKING_DIRECTORY ${jemalloc_SOURCE_DIR}
      OUTPUT_QUIET
    )
    execute_process(
      COMMAND make install
      WORKING_DIRECTORY ${jemalloc_SOURCE_DIR}
      OUTPUT_QUIET
    )

    set(jemalloc_POPULATED
        ON
        CACHE BOOL "set it to prevent building again"
    )
    set(Jemalloc_ROOT
        ${JEMALLOC_INSTALL_DIR}
        CACHE PATH "Path for Jemalloc"
    )
  endif()

  # Tell CMake to look in our staging area for find_package()
  list(APPEND CMAKE_PREFIX_PATH "${JEMALLOC_INSTALL_DIR}")

  fetchcontent_declare(
    hpx
    GIT_REPOSITORY ${hpx_repository}
    GIT_TAG master
    GIT_PROGRESS TRUE
    GIT_SHALLOW 1
  )
  set(HPX_WITH_FETCH_ASIO
      ON
      CACHE BOOL "FETCH ASIO for HPX"
  )
  set(HPX_WITH_FETCH_BOOST
      OFF
      CACHE BOOL "FETCH BOOST for HPX"
  )
  set(HPX_WITH_FETCH_HWLOC
      ON
      CACHE BOOL "FETCH HWLOC for HPX"
  )
  set(HPX_WITH_EXAMPLES
      OFF
      CACHE BOOL "Examples for HPX"
  )
  set(HPX_WITH_TESTS
      OFF
      CACHE BOOL "Tests for HPX"
  )
  set(HPX_WITH_MALLOC
      jemalloc
      CACHE STRING "Malloc type for HPX"
  )

  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_SYSTEM_PROCESSOR MATCHES
                                                 "arm64"
  )
    set(HPX_WITH_GENERIC_CONTEXT_COROUTINES
        ON
        CACHE BOOL "Enable generic context coroutines on AArch64" FORCE
    )
    message(STATUS "HPX_WITH_GENERIC_CONTEXT_COROUTINES: ON")
  endif()
  fetchcontent_makeavailable(hpx)
else()
  find_package(HPX REQUIRED CONFIG)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# create library target
set(chplx_library_detail_headers include/chplx/detail/generate_annotation.hpp
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

set(chplx_library_detail_sources src/detail/generate_annotation.cpp)
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

add_library(
  library STATIC ${chplx_library_sources} ${chplx_library_detail_sources}
                 ${chplx_library_headers}
)

set_property(TARGET library PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(library PRIVATE fmt::fmt-header-only HPX::hpx)

target_include_directories(
  library PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
                 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
# "$<IF:$<BOOL:INSTALL_INTERFACE>,${CMAKE_INSTALL_PREFIX}/include,${CMAKE_CURRENT_SOURCE_DIR}/include>/chplx/config/version.hpp"
# if($<BOOL:INSTALL_INTERFACE>) configure_file(
# "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/version.hpp.in"
# "${CMAKE_INSTALL_PREFIX}/include/chplx/config/version.hpp" @ONLY ) endif()
# if($<BOOL:BUILD_INTERFACE>)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/chplx/config/version.hpp" @ONLY
)
# endif()

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

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include" # source directory
  DESTINATION "${CMAKE_INSTALL_PREFIX}/" # target directory
  # FILES_MATCHING # install only matched files PATTERN "*.h" # select header
  # files PATTERN "*.hpp" # select header files
)

install(
  TARGETS
  library EXPORT ChplxTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  EXPORT ChplxTargets
  NAMESPACE Chplx::
  FILE ChplxConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Chplx
)
