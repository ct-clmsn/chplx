//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution.hpp>

namespace chplx {

template <typename F, typename... Args> void begin(F &&f, Args &&...args) {

  hpx::parallel::execution::post(hpx::execution::par.executor(),
                                 std::forward<F>(f),
                                 std::forward<Args>(args)...);
}
} // namespace chplx
