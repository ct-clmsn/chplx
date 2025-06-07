# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

macro(chplx_complement_sourcepath file input_path)

  # If input is not a full path, it's in the current source directory.
  get_filename_component(dir ${file} DIRECTORY)

  if(dir STREQUAL "")
    set(${input_path} "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
  else()
    set(${input_path} "${file}")
  endif()
endmacro()

macro(chplx_complement_binarypath name file input_path)

  # If input is not a full path, it's in the current source directory.
  get_filename_component(dir ${file} DIRECTORY)

  if(dir STREQUAL "")
    set(dir ${CMAKE_CURRENT_BINARY_DIR})
  endif()
  set(${input_path} "${dir}/${name}/${file}")
endmacro()

macro(chplx_complement_goodpath name file good_path)

  get_filename_component(dir ${file} DIRECTORY)
  get_filename_component(filename ${file} NAME_WE)
  get_filename_component(ext ${file} EXT)

  if(dir STREQUAL "")
    set(dir "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
  set(${good_path} "${dir}/${name}/${filename}${ext}.good")

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
  chplx_complement_sourcepath(${${name}_SOURCE} input_path)

  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/${name}")
  file(MAKE_DIRECTORY ${output_dir})
  add_custom_command(
    OUTPUT ${output_dir}/${name}.cpp ${output_dir}/${name}.hpp
    COMMAND "$<TARGET_FILE:chplx>" "-f" "${input_path}" "-F"
    DEPENDS "${${name}_SOURCE}" ${${name}_DEPENDENCIES}
    WORKING_DIRECTORY "${output_dir}"
    COMMENT "Compiling Chapel file ${${name}_SOURCE}."
    VERBATIM
  )

  add_custom_target(
    ${name}_compile_chpl
    DEPENDS ${output_dir}/${name}.cpp ${output_dir}/${name}.hpp
            ${${name}_DEPENDENCIES}
  )

  set_target_properties(${name}_compile_chpl
    PROPERTIES
      FOLDER "Backend/Tests/Compile"
      SOURCES ${input_path}
  )
  source_group("Source Files" FILES ${input_path})

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

  set(result_paths)
  foreach(file ${${name}_SOURCES})
    chplx_complement_binarypath(${name} ${file} input_path)

    add_custom_command(
      TARGET ${name}_compile_chpl
      POST_BUILD
      COMMAND "${CLANGFORMAT_EXECUTABLE}" "-i" "${input_path}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}"
      VERBATIM
    )

    # add_dependencies(${name}_compile_chpl "${input_path}" ${${name}_DEPENDENCIES})

    # COMMENT "Clang-formatting file ${input_path}.\n"

    set(result_paths "${result_paths}" "${input_path}")
  endforeach()
endfunction()

function(chplx_compare_result name)
  set(options)
  set(one_value_args GOOD TEST_NAME)
  set(multi_value_args DEPENDENCIES SOURCES)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_SOURCES)
    chplx_error("chplx_compare_result needs at least one SOURCES argument")
  endif()
  if(NOT ${name}_TEST_NAME)
    set(${name}_TEST_NAME ${name}_test)
  endif()

  set(source_paths)
  set(good_paths)

  foreach(src ${${name}_SOURCES})
    # If input is not a full path, it's in the current source directory.
    chplx_complement_binarypath(${name} ${src} source_path)
    set(source_paths ${source_paths} ${source_path})

    chplx_complement_goodpath(${name} ${src} good_path)
    set(good_paths ${good_paths} ${good_path})
  endforeach()

  string(REPLACE ";" "\\;" escaped_source_paths "${source_paths}")
  string(REPLACE ";" "\\;" escaped_good_paths "${good_paths}")

  set(cmake_arguments "-DBLESSED_OUTPUTS=${escaped_good_paths}"
                      "-DTEST_OUTPUTS=${escaped_source_paths}"
  )

  add_test(
    NAME ${${name}_TEST_NAME}
    COMMAND ${CMAKE_COMMAND} ${cmake_arguments} -P
            ${PROJECT_SOURCE_DIR}/cmake/CompareFiles.cmake
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}"
  )

  set(${name}_dependencies ${name}_compile_chpl ${${name}_DEPENDENCIES})
  set_tests_properties(
    ${${name}_TEST_NAME} PROPERTIES DEPENDS "${${name}_dependencies}"
  )

endfunction()

function(chplx_compile_project name)
  set(options)
  set(one_value_args TARGET TEST_NAME)
  set(multi_value_args DEPENDENCIES)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_TARGET)
    chplx_error("chplx_compile_project needs one TARGET argument")
  endif()
  if(NOT ${name}_TEST_NAME)
    set(${name}_TEST_NAME ${name}_test)
  endif()

  if(CMAKE_GENERATOR_TOOLSET)
    set(generator_toolset -T ${CMAKE_GENERATOR_TOOLSET})
  endif()

  add_custom_command(
    COMMAND ${CMAKE_COMMAND} . -B ./build
        -G ${CMAKE_GENERATOR} ${generator_toolset}
        "-Dfmt_DIR=${fmt_DIR}"
        "-DHPX_DIR=${HPX_DIR}"
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DChplx_DIR=$<TARGET_PROPERTY:library,TARGET_BINARY_DIR>/lib/cmake/Chplx"
        "-DHPX_IGNORE_COMPILER_COMPATIBILITY=On"
    COMMAND ${CMAKE_COMMAND} --build ./build
    COMMAND ${CMAKE_COMMAND}
        -E touch "${CMAKE_CURRENT_BINARY_DIR}/${name}/build/timestamp"
    DEPENDS ${name}_compile_chpl ${${name}_DEPENDENCIES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}"
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}/build/timestamp"
    VERBATIM
  )

  add_custom_target(${name}_compile_cpp
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}/build/timestamp"
  )

  set_target_properties(${name}_compile_cpp
    PROPERTIES
      FOLDER "Backend/Tests/Build"
  )

  add_test(
    NAME ${${name}_TEST_NAME}
    COMMAND ${CMAKE_CTEST_COMMAND} --test_dir ./build -C ${CMAKE_BUILD_TYPE}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}"
  )

  set(${name}_dependencies ${name}_compile_cpp)
  set_tests_properties(
    ${${name}_TEST_NAME} PROPERTIES DEPENDS "${${name}_compile_cpp}"
  )

endfunction()
