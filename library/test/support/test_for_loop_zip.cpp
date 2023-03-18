//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

#include <cstddef>
#include <set>

template <typename... Rs> void testForLoopZip(Rs &&...rs) {

  auto zip = chplx::zip(std::forward<Rs>(rs)...);

  std::set<typename decltype(zip)::indexType> values;

  std::size_t count = 0;

  chplx::forLoop(zip, [&](auto value) {
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(zip.size()));

  count = 0;
  for (auto const &e : zip.these()) {

    ++count;
    HPX_TEST(values.contains(e));
  }

  HPX_TEST_EQ(static_cast<std::size_t>(count), values.size());
}

int main() {

  testForLoopZip(chplx::Range(0, 10));
  testForLoopZip(chplx::Range(0, 10), chplx::Range(0, 10));
  testForLoopZip(chplx::Range(0, 10), chplx::Range(0, 10), chplx::Range(0, 10));

  testForLoopZip(chplx::BoundedRange<int, true>(0, 10, 2), chplx::Range(0, 10));
  testForLoopZip(chplx::Range(0, 10, chplx::BoundsCategoryType::Open),
                 chplx::Range(0, 10));
  testForLoopZip(
      chplx::BoundedRange<int, true>(0, 10, 2, chplx::BoundsCategoryType::Open),
      chplx::Range(0, 10));

  testForLoopZip(chplx::Range(1, 0), chplx::Range(0, 10));

  testForLoopZip(chplx::BoundedRange<int, true>(1, 9, 2), chplx::Range(0, 10));
  testForLoopZip(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open),
      chplx::Range(0, 10));

  testForLoopZip(by(chplx::BoundedRange<int, true>(1, 10), -1),
                 chplx::Range(0, 10));
  testForLoopZip(by(chplx::BoundedRange<int, true>(1, 10), -2),
                 chplx::Range(0, 10));

  return hpx::util::report_errors();
}
