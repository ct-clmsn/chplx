/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <string>

namespace chplx::util {

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

// emit line directive
std::string emitLineDirective(char const* name, int line);
} // namespace chplx::util
