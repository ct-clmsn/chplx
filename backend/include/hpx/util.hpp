/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace chplx::util {

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;
extern bool compilerDebug;
extern std::filesystem::path output_path;

extern std::vector<std::filesystem::path> incdirs;
extern std::vector<std::filesystem::path> libdirs;
extern std::vector<std::string> libs; 
extern std::vector<std::string> flagscxx;
extern std::vector<std::string> packages_cmake;
extern std::vector<std::string> packages_pkgconfig;

// emit line directive
std::string emitLineDirective(char const* name, int line);
} // namespace chplx::util
