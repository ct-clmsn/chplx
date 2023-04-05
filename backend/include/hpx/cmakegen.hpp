/*
 * Copyright (c) 2023 Christopher Taylor
 *
 * SPDX-License-Identifier: BSL-1.0
 * Distributed under the Boost Software License, Version 1.0. *(See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#ifndef __CHPLX_CMAKEGENERATOR_HPP__
#define __CHPLX_CMAKEGENERATOR_HPP__

#include <filesystem>

struct CMakeGenerator {
    void generate(std::filesystem::path const& p);
};

#endif
