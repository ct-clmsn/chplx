//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/execution.hpp>
#include <hpx/parallel/task_group.hpp>

namespace chplx {

template <typename F, typename... Fs> void cobegin(F &&f, Fs &&...fs) {

  auto exec = hpx::execution::par.executor();
  hpx::experimental::task_group g;

  g.run(exec, std::forward<F>(f));
  (g.run(exec, std::forward<Fs>(fs)), ...);

  g.wait();
}
} // namespace chplx
