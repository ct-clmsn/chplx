# Copyright (c) 2023 Hartmut Kaiser
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. *(See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if(NOT BLESSED_OUTPUTS)
  message(SEND_ERROR "Variable BLESSED_OUTPUTS not provided")
endif()

if(NOT TEST_OUTPUTS)
  message(SEND_ERROR "Variable TEST_OUTPUTS not provided")
endif()

list(LENGTH BLESSED_OUTPUTS blessed_len)
list(LENGTH TEST_OUTPUTS test_len)
if(NOT (${blessed_len} EQUAL ${test_len}))
  message(
    SEND_ERROR "BLESSED_OUTPUT should have the same lengths as TEST_OUTPUT"
  )
endif()

math(EXPR len "${blessed_len} - 1")
foreach(val RANGE ${len})
  list(GET BLESSED_OUTPUTS ${val} blessed_output)
  list(GET TEST_OUTPUTS ${val} test_output)

  execute_process(
    COMMAND diff -w ${test_output} ${blessed_output}
    RESULT_VARIABLE test_result
    OUTPUT_VARIABLE diff_output
    ERROR_VARIABLE diff_error
  )

  if(test_result)
    file(READ ${test_output} test_output_content)
    file(READ ${blessed_output} blessed_output_content)
    message(SEND_ERROR "Output mismatch (ignoring whitespace):\nDiff:\n${diff_output}\n\nTest Output:\n${test_output_content}\n\nExpected Output:\n${blessed_output_content}")
  endif()
endforeach()
