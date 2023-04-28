//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// implement setenv for MSVC

#if defined(_MSC_VER)

#if defined(__cplusplus)
extern "C" {
#endif

int setenv(const char *name, const char *value, int overwrite);

#if defined(__cplusplus)
}
#endif

#else

#include <stdlib.h>

#endif
