# Copyright (c) 2011 Bryce Lelbach
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

function(add_chplx_pseudo_target)
  chplx_debug("add_chplx_pseudo_target" "adding pseudo target: ${ARGV}")
  if(CHPLX_WITH_PSEUDO_DEPENDENCIES)
    set(shortened_args)
    foreach(arg ${ARGV})
      shorten_chplx_pseudo_target(${arg} shortened_arg)
      set(shortened_args ${shortened_args} ${shortened_arg})
    endforeach()
    chplx_debug("add_chplx_pseudo_target"
              "adding shortened pseudo target: ${shortened_args}"
    )
    add_custom_target(${shortened_args})
  endif()
endfunction()
