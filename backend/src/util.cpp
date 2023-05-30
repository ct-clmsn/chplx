/*
 * Copyright (c) 2023 Hartmut Kaiser
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include <hpx/util.hpp>

#include <string>
#include <filesystem>
#include <vector>

#include <fmt/core.h>

namespace chplx::util {

// global options
bool suppressLineDirectives = false;
bool fullFilePath = true;
bool compilerDebug = false;
std::filesystem::path output_path = {};

std::vector<std::filesystem::path> incdirs = {};
std::vector<std::filesystem::path> libdirs = {};
std::vector<std::string> libs = {};
std::vector<std::string> flagscxx = {};

std::vector<std::string> packages_cmake = {};
std::vector<std::string> packages_pkgconfig = {};

// emit line directive
std::string emitLineDirective(char const* name, int line) {
  if (!suppressLineDirectives) {
    std::filesystem::path p(name);
    return fmt::format("#line {0} \"{1}\"\n", line,
                       (fullFilePath ? p : p.filename()).string());
  }
  return {};
}
} // namespace chplx::util
