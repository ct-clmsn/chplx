# Copyright (c) 2011 Bryce Lelbach
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

macro(chplx_include)
  foreach(listfile ${ARGV})
    include("CHPLX_${listfile}")
  endforeach()
endmacro()
