# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

include(CHPLX_LibraryAddTest)

set(tests
    # array
    assoc_domain
    atomic
    begin
    cobegin
    coforall_loop_array
    coforall_loop_assoc_domain
    coforall_loop_domain
    # coforall_loop_range
    coforall_loop_tuple
    coforall_loop_zip
    domain
    forall_loop_array
    forall_loop_assoc_domain
    forall_loop_domain
    # forall_loop_range
    forall_loop_tuple
    forall_loop_zip
    for_loop_array
    for_loop_assoc_domain
    for_loop_domain
    # for_loop_range
    for_loop_tuple
    for_loop_zip
    index_buffer
    # index_generator
    # locales
    # print_ranges
    # range
    # single_variable
    sync_variable
    tuple
    # write_array
    zip
)

set(locales_PARAMETERS LOCALITIES 2)

foreach(test ${tests})
  chplx_library_add_unit_test(${test} ${${test}_PARAMETERS})
endforeach()
