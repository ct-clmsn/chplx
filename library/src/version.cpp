//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx/version.hpp>
#include <chplx/config/version.hpp>

#include <fmt/format.h>

#include <string>

namespace chplx {

    std::uint8_t majorVersion()
    {
        return CHPLX_VERSION_MAJOR;
    }

    std::uint8_t minorVersion()
    {
        return CHPLX_VERSION_MINOR;
    }

    std::uint8_t patchVersion()
    {
        return CHPLX_VERSION_PATCH;
    }

    std::uint32_t fullVersion()
    {
        return CHPLX_VERSION_FULL;
    }

    std::string fullVersionAsString()
    {
        return fmt::format("{}.{}.{}",
            CHPLX_VERSION_MAJOR, CHPLX_VERSION_MINOR, CHPLX_VERSION_PATCH);
    }
} // namespace chplx
