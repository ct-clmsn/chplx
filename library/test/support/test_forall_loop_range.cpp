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
void testForallLoopRange(chplx::Range<T, BoundedType, Stridable> r) {

  T count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::forallLoop(r, [&](auto value) {
    std::lock_guard l(mtx);
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, r.size());
  count = 0;

  for (auto val : r.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, r.size());
}

int main() {

  testForallLoopRange(chplx::Range(0, 10));
  testForallLoopRange(chplx::BoundedRange<int, true>(0, 10, 2));
  testForallLoopRange(chplx::Range(0, 10, chplx::BoundsCategoryType::Open));
  testForallLoopRange(chplx::BoundedRange<int, true>(
      0, 10, 2, chplx::BoundsCategoryType::Open));

  testForallLoopRange(chplx::Range(1, 0));

  testForallLoopRange(chplx::BoundedRange<int, true>(1, 9, 2));
  testForallLoopRange(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open));

  testForallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -1));
  testForallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -2));

  return hpx::util::report_errors();
}
