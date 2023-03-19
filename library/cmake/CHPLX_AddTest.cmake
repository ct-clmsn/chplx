# Copyright (c) 2011 Bryce Lelbach
# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function(add_chplx_test category name)
  set(options FAILURE_EXPECTED RUN_SERIAL NO_PARCELPORT_TCP NO_PARCELPORT_MPI
              NO_PARCELPORT_LCI
  )
  set(one_value_args EXECUTABLE LOCALITIES THREADS_PER_LOCALITY TIMEOUT
                     RUNWRAPPER
  )
  set(multi_value_args ARGS PARCELPORTS)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_LOCALITIES)
    set(${name}_LOCALITIES 1)
  endif()

  if(NOT ${name}_THREADS_PER_LOCALITY)
    set(${name}_THREADS_PER_LOCALITY 1)
  elseif(CHPLX_WITH_TESTS_MAX_THREADS_PER_LOCALITY GREATER 0
         AND ${name}_THREADS_PER_LOCALITY GREATER
             CHPLX_WITH_TESTS_MAX_THREADS_PER_LOCALITY
  )
    set(${name}_THREADS_PER_LOCALITY
        ${CHPLX_WITH_TESTS_MAX_THREADS_PER_LOCALITY}
    )
  endif()

  if(NOT ${name}_EXECUTABLE)
    set(${name}_EXECUTABLE ${name})
  endif()

  if(TARGET ${${name}_EXECUTABLE}_test)
    set(_exe "$<TARGET_FILE:${${name}_EXECUTABLE}_test>")
  elseif(TARGET ${${name}_EXECUTABLE})
    set(_exe "$<TARGET_FILE:${${name}_EXECUTABLE}>")
  else()
    set(_exe "${${name}_EXECUTABLE}")
  endif()

  set(expected "0")

  if(${name}_FAILURE_EXPECTED)
    set(expected "1")
  endif()

  if(${name}_RUN_SERIAL)
    set(run_serial TRUE)
  endif()

  # If --hpx:threads=cores or all
  if(${name}_THREADS_PER_LOCALITY LESS_EQUAL 0)
    set(run_serial TRUE)
  endif()

  set(args)

  foreach(arg ${${name}_UNPARSED_ARGUMENTS})
    set(args ${args} "${arg}")
  endforeach()
  set(args "-v" "--" ${args})
  if(${CHPLX_WITH_TESTS_DEBUG_LOG})
    set(args ${args}
             "--hpx:debug-hpx-log=${CHPLX_WITH_TESTS_DEBUG_LOG_DESTINATION}"
    )
  endif()

  if(${CHPLX_WITH_PARALLEL_TESTS_BIND_NONE}
     AND NOT run_serial
     AND NOT "${name}_RUNWRAPPER"
     AND (${name}_LOCALITIES STREQUAL "1" OR NOT ${CHPLX_WITH_NETWORKING})
  )
    set(args ${args} "--hpx:bind=none")
  endif()

  if(CHPLX_WITH_INSTALLED_VERSION)
    set(_script_location ${CHPLX_PREFIX})
  else()
    set(_script_location ${PROJECT_BINARY_DIR})
  endif()

  # cmake-format: off
  set(cmd
      "${PYTHON_EXECUTABLE}"
      "${_script_location}/bin/chplxrun.py"
      ${CMAKE_CROSSCOMPILING_EMULATOR}
      ${_exe}
        "-e" "${expected}"
        "-t" "${${name}_THREADS_PER_LOCALITY}"
  )
  # cmake-format: on

  # if runwrapper is set we don't want to use HPX parcelports
  if(${name}_RUNWRAPPER)
    list(
      APPEND
      cmd
      "-r"
      "${${name}_RUNWRAPPER}"
      "-l"
      "${${name}_LOCALITIES}"
      "-p"
      "none"
    )
    set(${name}_LOCALITIES "1")
  elseif(CHPLX_WITH_NETWORKING)
    list(APPEND cmd "-l" "${${name}_LOCALITIES}")
  else()
    set(${name}_LOCALITIES "1")
  endif()

  if(${name}_LOCALITIES STREQUAL "1")
    set(_full_name "${category}.${name}")
    add_test(NAME "${category}.${name}" COMMAND ${cmd} ${args})
    if(${run_serial})
      set_tests_properties("${_full_name}" PROPERTIES RUN_SERIAL TRUE)
    endif()
    if(${name}_TIMEOUT)
      set_tests_properties(
        "${_full_name}" PROPERTIES TIMEOUT ${${name}_TIMEOUT}
      )
    endif()
  else()
    if(CHPLX_WITH_PARCELPORT_MPI AND NOT ${${name}_NO_PARCELPORT_MPI})
      set(_add_test FALSE)
      if(DEFINED ${name}_PARCELPORTS)
        set(PP_FOUND -1)
        list(FIND ${name}_PARCELPORTS "mpi" PP_FOUND)
        if(NOT PP_FOUND EQUAL -1)
          set(_add_test TRUE)
        endif()
      else()
        set(_add_test TRUE)
      endif()
      if(_add_test)
        set(_full_name "${category}.distributed.mpi.${name}")
        add_test(NAME "${_full_name}" COMMAND ${cmd} "-p" "mpi" "-r" "mpi"
                                              ${args}
        )
        set_tests_properties("${_full_name}" PROPERTIES RUN_SERIAL TRUE)
        if(${name}_TIMEOUT)
          set_tests_properties(
            "${_full_name}" PROPERTIES TIMEOUT ${${name}_TIMEOUT}
          )
        endif()
      endif()
    endif()
    if(CHPLX_WITH_PARCELPORT_LCI AND NOT ${${name}_NO_PARCELPORT_LCI})
      set(_add_test FALSE)
      if(DEFINED ${name}_PARCELPORTS)
        set(PP_FOUND -1)
        list(FIND ${name}_PARCELPORTS "lci" PP_FOUND)
        if(NOT PP_FOUND EQUAL -1)
          set(_add_test TRUE)
        endif()
      else()
        set(_add_test TRUE)
      endif()
      if(_add_test)
        set(_full_name "${category}.distributed.lci.${name}")
        add_test(NAME "${_full_name}" COMMAND ${cmd} "-p" "lci" "-r" "mpi"
                                              ${args}
        )
        set_tests_properties("${_full_name}" PROPERTIES RUN_SERIAL TRUE)
        if(${name}_TIMEOUT)
          set_tests_properties(
            "${_full_name}" PROPERTIES TIMEOUT ${${name}_TIMEOUT}
          )
        endif()
      endif()
    endif()
    if(CHPLX_WITH_PARCELPORT_TCP AND NOT ${${name}_NO_PARCELPORT_TCP})
      set(_add_test FALSE)
      if(DEFINED ${name}_PARCELPORTS)
        set(PP_FOUND -1)
        list(FIND ${name}_PARCELPORTS "tcp" PP_FOUND)
        if(NOT PP_FOUND EQUAL -1)
          set(_add_test TRUE)
        endif()
      else()
        set(_add_test TRUE)
      endif()
      if(_add_test)
        set(_full_name "${category}.distributed.tcp.${name}")
        add_test(NAME "${_full_name}" COMMAND ${cmd} "-p" "tcp" ${args})
        set_tests_properties("${_full_name}" PROPERTIES RUN_SERIAL TRUE)
        if(${name}_TIMEOUT)
          set_tests_properties(
            "${_full_name}" PROPERTIES TIMEOUT ${${name}_TIMEOUT}
          )
        endif()
      endif()
    endif()
  endif()
endfunction()


function(add_chplx_test_target_dependencies category name)
  set(one_value_args PSEUDO_DEPS_NAME)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )
  # default target_extension is _test but for examples.* target, it may vary
  if(NOT ("${category}" MATCHES "tests.examples*"))
    set(_ext "_test")
  endif()
  # Add a custom target for this example
  add_chplx_pseudo_target(${category}.${name})
  # Make pseudo-targets depend on master pseudo-target
  add_chplx_pseudo_dependencies(${category} ${category}.${name})
  # Add dependencies to pseudo-target
  if(${name}_PSEUDO_DEPS_NAME)
    # When the test depend on another executable name
    add_chplx_pseudo_dependencies(
      ${category}.${name} ${${name}_PSEUDO_DEPS_NAME}${_ext}
    )
  else()
    add_chplx_pseudo_dependencies(${category}.${name} ${name}${_ext})
  endif()
endfunction()


# To add test to the category root as in tests/regressions/ with correct name
function(add_chplx_test_and_deps_test category subcategory name)
  if("${subcategory}" STREQUAL "")
    add_chplx_test(tests.${category} ${name} ${ARGN})
    add_chplx_test_target_dependencies(tests.${category} ${name} ${ARGN})
  else()
    add_chplx_test(tests.${category}.${subcategory} ${name} ${ARGN})
    add_chplx_test_target_dependencies(
      tests.${category}.${subcategory} ${name} ${ARGN}
    )
  endif()
endfunction()

function(add_chplx_unit_test subcategory name)
  add_chplx_test_and_deps_test("unit" "${subcategory}" ${name} ${ARGN})
endfunction(add_chplx_unit_test)

