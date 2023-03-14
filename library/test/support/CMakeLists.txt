# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests test_index_generator
    test_coforall_loop_domain
    test_coforall_loop_range
    test_coforall_loop_tuple
    test_coforall_loop_zip
    test_forall_loop_domain
    test_forall_loop_range
    test_forall_loop_tuple
    test_forall_loop_zip
    test_for_loop_domain
    test_for_loop_range
    test_for_loop_tuple
    test_for_loop_zip
    test_print_range
    test_domain
    test_range
    test_tuple
    test_zip
)

foreach(test ${tests})
  chplx_unit_test(${test})
endforeach()
