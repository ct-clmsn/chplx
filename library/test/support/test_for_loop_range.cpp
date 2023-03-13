//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void testForLoopRange(chplx::Range<T, BoundedType, Stridable> r) {

  int called = 0;

  T val = r.first();
  T stride = r.stride();

  chplx::forLoop(r, [&](auto value) {
    ++called;
    HPX_TEST_EQ(value, val);
    val += stride;
  });

  HPX_TEST_EQ(called, r.size());
}

int main() {

  testForLoopRange(chplx::Range(0, 10));
  testForLoopRange(chplx::BoundedRange<int, true>(0, 10, 2));
  testForLoopRange(chplx::Range(0, 10, chplx::BoundsCategoryType::Open));
  testForLoopRange(chplx::BoundedRange<int, true>(
      0, 10, 2, chplx::BoundsCategoryType::Open));

  testForLoopRange(chplx::Range(1, 0));

  testForLoopRange(chplx::BoundedRange<int, true>(1, 9, 2));
  testForLoopRange(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open));

  testForLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -1));
  testForLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -2));

  return hpx::util::report_errors();
}
