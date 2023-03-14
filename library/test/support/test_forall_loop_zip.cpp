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

template <typename... Rs> void testForallLoopZip(Rs &&...rs) {

  std::size_t count = 0;
  auto zip = chplx::zip(std::forward<Rs>(rs)...);

  std::set<typename decltype(zip)::indexType> values;
  hpx::mutex mtx;

  chplx::forallLoop(zip, [&](auto value) {
    std::lock_guard l(mtx);
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(zip.size()));
  count = 0;

  for (auto val : zip.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, values.size());
}

int main() {

  testForallLoopZip(chplx::Range(0, 10));
  testForallLoopZip(chplx::Range(0, 10), chplx::Range(0, 10));
  testForallLoopZip(chplx::Range(0, 10), chplx::Range(0, 10),
                    chplx::Range(0, 10));

  testForallLoopZip(chplx::BoundedRange<int, true>(0, 10, 2),
                    chplx::Range(0, 10));
  testForallLoopZip(chplx::Range(0, 10, chplx::BoundsCategoryType::Open),
                    chplx::Range(0, 10));
  testForallLoopZip(
      chplx::BoundedRange<int, true>(0, 10, 2, chplx::BoundsCategoryType::Open),
      chplx::Range(0, 10));

  testForallLoopZip(chplx::Range(1, 0), chplx::Range(0, 10));

  testForallLoopZip(chplx::BoundedRange<int, true>(1, 9, 2),
                    chplx::Range(0, 10));
  testForallLoopZip(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open),
      chplx::Range(0, 10));

  testForallLoopZip(by(chplx::BoundedRange<int, true>(1, 10), -1),
                    chplx::Range(0, 10));
  testForallLoopZip(by(chplx::BoundedRange<int, true>(1, 10), -2),
                    chplx::Range(0, 10));

  return hpx::util::report_errors();
}
