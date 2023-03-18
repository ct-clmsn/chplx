//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/synchronization.hpp>
#include <hpx/modules/testing.hpp>

#include <atomic>

int main() {

  std::atomic<int> count(0);

  hpx::latch l(5);

  auto f = [&]() {
    ++count;
    l.count_down(1);
  };

  chplx::begin(f);
  chplx::begin(f);
  chplx::begin(f);
  chplx::begin(f);

  l.arrive_and_wait();

  HPX_TEST_EQ(count.load(), 4);

  return hpx::util::report_errors();
}
