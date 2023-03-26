# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function(add_chplx_pseudo_target_folder name)
  set(options)
  set(one_value_args FOLDER TARGET)
  set(multi_value_args)
  cmake_parse_arguments(
    ${name} "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(NOT ${name}_FOLDER OR NOT ${name}_TARGET)
    chplx_error(
      "add_chplx_pseudo_target_folder needs one FOLDER and one TARGET argument"
    )
  endif()

  chplx_debug("add_chplx_pseudo_target_folder" "adding pseudo target to FOLDER: ${ARGV}")
  if(CHPLX_WITH_PSEUDO_DEPENDENCIES)
    shorten_chplx_pseudo_target(${${name}_TARGET} shortened_arg)
    chplx_debug("add_chplx_pseudo_target_folder"
              "adding shortened pseudo target: ${shortened_arg}"
    )
    set_target_properties(${shortened_arg} PROPERTIES FOLDER "${${name}_FOLDER}")
  endif()
endfunction()
