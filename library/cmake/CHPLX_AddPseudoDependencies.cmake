# Copyright (c) 2011 Bryce Lelbach
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CHPLX_Message)

function(add_chplx_pseudo_dependencies)

  if("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "DEBUG|debug|Debug")
    set(args)
    foreach(arg ${ARGV})
      set(args "${args} ${arg}")
    endforeach()
    chplx_debug("add_chplx_pseudo_dependencies" ${args})
  endif()

  if(CHPLX_WITH_PSEUDO_DEPENDENCIES)
    set(shortened_args)
    foreach(arg ${ARGV})
      shorten_chplx_pseudo_target(${arg} shortened_arg)
      set(shortened_args ${shortened_args} ${shortened_arg})
    endforeach()
    add_dependencies(${shortened_args})
  endif()
endfunction()

function(add_chplx_pseudo_dependencies_no_shortening)

  if("${CHPLX_CMAKE_LOGLEVEL}" MATCHES "DEBUG|debug|Debug")
    set(args)
    foreach(arg ${ARGV})
      set(args "${args} ${arg}")
    endforeach()
    chplx_debug("add_chplx_pseudo_dependencies" ${args})
  endif()

  if(CHPLX_WITH_PSEUDO_DEPENDENCIES)
    add_dependencies(${ARGV})
  endif()
endfunction()
