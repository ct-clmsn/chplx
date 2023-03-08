//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/thread.hpp>

#include <mutex>
#include <set>

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void test_coforall_loop_range(chplx::Range<T, BoundedType, Stridable> r) {

  int called = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::coforallLoop(r, [&](auto value) {
    std::lock_guard l(mtx);
    ++called;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  std::size_t count = 0;

  for (auto val : chplx::iterate(r)) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, r.size());
  HPX_TEST_EQ(called, r.size());
}

int main() {

  test_coforall_loop_range(chplx::Range(0, 10));
  test_coforall_loop_range(chplx::BoundedRange<int, true>(0, 10, 2));
  test_coforall_loop_range(
      chplx::Range(0, 10, chplx::BoundsCategoryType::Open));
  test_coforall_loop_range(chplx::BoundedRange<int, true>(
      0, 10, 2, chplx::BoundsCategoryType::Open));

  test_coforall_loop_range(chplx::Range(1, 0));

  test_coforall_loop_range(by(chplx::BoundedRange<int, true>(1, 10), -1));
  test_coforall_loop_range(by(chplx::BoundedRange<int, true>(1, 10), -2));

  return hpx::util::report_errors();
}
