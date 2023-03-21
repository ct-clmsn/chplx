# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

macro(chplx_complement_filepath file input_path)

  # If input is not a full path, it's in the current source directory.
  get_filename_component(input_path ${file} PATH)

  if(input_path STREQUAL "")
    set(input_path "${CMAKE_CURRENT_SOURCE_DIR}/${${name}_SOURCE}")
  else()
    set(input_path "${${name}_SOURCE}")
  endif()
endmacro()

# Compile .chpl file to C++
function(chplx_compile name)
  set(options)
  set(one_value_args SOURCE)
  set(multi_value_args DEPENDENCIES)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_SOURCE)
    chplx_error("chplx_compile needs one SOURCE argument")
  endif()

  # If input is not a full path, it's in the current source directory.
  chplx_complement_filepath(${${name}_SOURCE} input_path)

  add_custom_command(
    OUTPUT ${name}.cpp
    COMMAND "$<TARGET_FILE:chplx>" "-f" "${input_path}"
    COMMENT "Compiling Chapel file ${${name}_SOURCE}."
    DEPENDS "${${name}_SOURCE}" ${${name}_DEPENDENCIES}
    VERBATIM
  )

  add_custom_target(
    ${name}_test DEPENDS ${name}.cpp DEPENDENCIES ${${name}_DEPENDENCIES}
  )

  set_target_properties(${name}_test PROPERTIES FOLDER "Backend/Tests")

endfunction()

# clang-format the given files
function(chplx_format_file name)
  set(options)
  set(one_value_args)
  set(multi_value_args DEPENDENCIES SOURCES)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_SOURCES)
    chplx_error("chplx_format_file needs at least one SOURCES argument")
  endif()

  foreach(file ${${name}_SOURCES})
    chplx_complement_filepath(${file} input_path)

    add_custom_command(
      TARGET ${name}_test
      POST_BUILD
      COMMAND "${CLANGFORMAT_EXECUTABLE}" "-i" "${input_path}"
      COMMENT "Clang-formatting file ${input_path}." DEPENDS "${input_path}"
              ${${name}_DEPENDENCIES}
      VERBATIM
    )

  endforeach()
endfunction()

function(chplx_compare_result name)
  set(options)
  set(one_value_args GOOD SOURCE)
  set(multi_value_args)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_SOURCE OR NOT ${name}_GOOD)
    chplx_error(
      "chplx_compare_result needs at one SOURCE and one GOOD argument"
    )
  endif()

  # If input is not a full path, it's in the current source directory.
  chplx_complement_filepath(${${name}_SOURCE} source_path)
  chplx_complement_filepath(${${name}_GOOD} good_path)

  add_test(
    NAME ${name}_compare
    COMMAND ${CMAKE_COMMAND} -E compare_files ${source_path} ${good_path}
  )
endfunction()
