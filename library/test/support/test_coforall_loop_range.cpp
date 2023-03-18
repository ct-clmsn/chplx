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
#include <ostream>

//-----------------------------------------------------------------------------
// helper for testing below
namespace chplx {

template <typename T> bool operator==(Sync<T> const &lhs, Sync<T> const &rhs) {
  return lhs.readFF() == rhs.readFF();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, Sync<T> const& s) {
  return os << s.readFF();
}

template <typename T>
bool operator==(Single<T> const &lhs, Single<T> const &rhs) {
  return lhs.readFF() == rhs.readFF();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, Single<T> const& s) {
  return os << s.readFF();
}

} // namespace chplx

//-----------------------------------------------------------------------------
template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable>
void testCoforallLoopRange(chplx::Range<T, BoundedType, Stridable> r) {

  std::size_t count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::coforall(r, [&](auto value) {
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

template <typename T, chplx::BoundedRangeType BoundedType, bool Stridable,
          typename Args>
void testCoforallLoopRange(chplx::Range<T, BoundedType, Stridable> r,
                           Args &&arg) {

  std::size_t count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::coforall(
      r,
      [&](auto value, auto &&farg) {
        HPX_TEST_EQ(arg, farg);
        std::lock_guard l(mtx);
        ++count;
        auto p = values.insert(value);
        HPX_TEST(p.second);
      },
      arg);

  HPX_TEST_EQ(count, static_cast<std::size_t>(r.size()));
  count = 0;

  for (auto val : r.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, values.size());
}

template <typename... Args> void testCoforallLoop(Args &&...args) {

  testCoforallLoopRange(chplx::Range(0, 10), args...);
  testCoforallLoopRange(chplx::BoundedRange<int, true>(0, 10, 2), args...);
  testCoforallLoopRange(chplx::Range(0, 10, chplx::BoundsCategoryType::Open),
                        args...);
  testCoforallLoopRange(
      chplx::BoundedRange<int, true>(0, 10, 2, chplx::BoundsCategoryType::Open),
      args...);

  testCoforallLoopRange(chplx::BoundedRange<int, true>(1, 9, 2), args...);
  testCoforallLoopRange(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open),
      args...);

  testCoforallLoopRange(chplx::Range(1, 0), args...);

  testCoforallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -1), args...);
  testCoforallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -2),
                        std::forward<Args>(args)...);
}

int main() {

  testCoforallLoop();
  testCoforallLoop(42);

  chplx::Sync<int> sy(42);
  testCoforallLoop(sy);

  chplx::Single<int> si(42);
  testCoforallLoop(si);

  return hpx::util::report_errors();
}
