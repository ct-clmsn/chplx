# Copyright (c) 2011 Bryce Lelbach
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function(_to_string var)
  set(_var "")

  foreach(_arg ${ARGN})
    string(REPLACE "\\" "/" _arg ${_arg})
    if("${_var}" STREQUAL "")
      set(_var "${_arg}")
    else()
      set(_var "${_var} ${_arg}")
    endif()
  endforeach()

  set(${var}
      ${_var}
      PARENT_SCOPE
  )
endfunction()

function(chplx_info)
  set(msg)
  _to_string(msg ${ARGN})
  message(STATUS "${msg}")
  unset(args)
endfunction()

function(chplx_debug)
  if("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "DEBUG|debug|Debug")
    set(msg)
    _to_string(msg ${ARGN})
    message(STATUS "DEBUG: ${msg}")
  endif()
endfunction()

function(chplx_warn)
  if("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "DEBUG|debug|Debug|WARN|warn|Warn")
    set(msg)
    _to_string(msg ${ARGN})
    message(STATUS "WARNING: ${msg}")
  endif()
endfunction()

function(chplx_error)
  set(msg)
  _to_string(msg ${ARGN})
  message(FATAL_ERROR "ERROR: ${msg}")
endfunction()

function(chplx_message level)
  if("${level}" MATCHES "ERROR|error|Error")
    chplx_error(${ARGN})
  elseif("${level}" MATCHES "WARN|warn|Warn")
    chplx_warn(${ARGN})
  elseif("${level}" MATCHES "DEBUG|debug|Debug")
    chplx_debug(${ARGN})
  elseif("${level}" MATCHES "INFO|info|Info")
    chplx_info(${ARGN})
  else(NOT("${level}" MATCHES "NONE|none|None"))
    chplx_error("message"
              "\"${level}\" is not an CHPLX configuration logging level."
    )
  endif()
endfunction()

function(chplx_config_loglevel level return)
  set(${return}
      FALSE
      PARENT_SCOPE
  )
  if("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "ERROR|error|Error"
     AND "${level}" MATCHES "ERROR|error|Error"
  )
    set(${return}
        TRUE
        PARENT_SCOPE
    )
  elseif("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "WARN|warn|Warn"
         AND "${level}" MATCHES "WARN|warn|Warn"
  )
    set(${return}
        TRUE
        PARENT_SCOPE
    )
  elseif("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "DEBUG|debug|Debug"
         AND "${level}" MATCHES "DEBUG|debug|Debug"
  )
    set(${return}
        TRUE
        PARENT_SCOPE
    )
  elseif("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "INFO|info|Info"
         AND "${level}" MATCHES "INFO|info|Info"
  )
    set(${return}
        TRUE
        PARENT_SCOPE
    )
  endif()
endfunction()

function(chplx_print_list level message list)
  chplx_config_loglevel(${level} printed)
  if(printed)
    if(${list})
      chplx_message(${level} "${message}: ")
      foreach(element ${${list}})
        message("    ${element}")
      endforeach()
    else()
      chplx_message(${level} "${message} is empty.")
    endif()
  endif()
endfunction()
