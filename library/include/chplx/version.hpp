//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include <string>

namespace chplx {

std::uint8_t majorVersion();
std::uint8_t minorVersion();
std::uint8_t patchVersion();
std::uint32_t fullVersion();
std::string fullVersionAsString();
} // namespace chplx
