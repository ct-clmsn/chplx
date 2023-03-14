//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>
#include <hpx/thread.hpp>

#include <cstddef>
#include <mutex>
#include <set>

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void testCoforallLoopRange(chplx::Range<T, BoundedType, Stridable> r) {

  std::size_t count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::coforallLoop(r, [&](auto value) {
    std::lock_guard l(mtx);
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(r.size()));
  count = 0;

  for (auto val : r.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, values.size());
}

int main() {

  testCoforallLoopRange(chplx::Range(0, 10));
  testCoforallLoopRange(chplx::BoundedRange<int, true>(0, 10, 2));
  testCoforallLoopRange(
      chplx::Range(0, 10, chplx::BoundsCategoryType::Open));
  testCoforallLoopRange(chplx::BoundedRange<int, true>(
      0, 10, 2, chplx::BoundsCategoryType::Open));

  testCoforallLoopRange(chplx::BoundedRange<int, true>(1, 9, 2));
  testCoforallLoopRange(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open));

  testCoforallLoopRange(chplx::Range(1, 0));

  testCoforallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -1));
  testCoforallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -2));

  return hpx::util::report_errors();
}
