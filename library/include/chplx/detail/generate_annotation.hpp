//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/modules/assertion.hpp>

#include <string>

namespace chplx::detail {

std::string generate_annotation(hpx::source_location const &location);
} // namespace chplx::detail
