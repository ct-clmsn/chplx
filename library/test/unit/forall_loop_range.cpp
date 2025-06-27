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


bool chplx_fork_join_executor = true;
hpx::execution::experimental::fork_join_executor *exec = nullptr;
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
void testForallLoopRange(chplx::Range<T, BoundedType, Stridable> r) {

  std::size_t count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::forall(r, [&](auto value) {
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
          typename Arg>
void testForallLoopRange(chplx::Range<T, BoundedType, Stridable> r, Arg &&arg) {

  std::size_t count = 0;

  std::set<T> values;
  hpx::mutex mtx;

  chplx::forall(
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

template <typename... Args> void testForallLoopRanges(Args &&...args) {

  testForallLoopRange(chplx::Range(0, 10), args...);
  testForallLoopRange(chplx::BoundedRange<int, true>(0, 10, 2), args...);
  testForallLoopRange(chplx::Range(0, 10, chplx::BoundsCategoryType::Open),
                      args...);
  testForallLoopRange(
      chplx::BoundedRange<int, true>(0, 10, 2, chplx::BoundsCategoryType::Open),
      args...);

  testForallLoopRange(chplx::Range(1, 0), args...);

  testForallLoopRange(chplx::BoundedRange<int, true>(1, 9, 2), args...);
  testForallLoopRange(
      chplx::BoundedRange<int, true>(1, 9, 2, chplx::BoundsCategoryType::Open),
      args...);

  testForallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -1), args...);
  testForallLoopRange(by(chplx::BoundedRange<int, true>(1, 10), -2),
                      std::forward<Args>(args)...);
}

int main() {

  chplx_fork_join_executor = true;
  exec = new hpx::execution::experimental::fork_join_executor();

  {
    testForallLoopRanges();
    testForallLoopRanges(42);

    chplx::Sync<int> sy(42);
    testForallLoopRanges(sy);

    chplx::Single<int> si(42);
    testForallLoopRanges(si);
  }

  delete exec;

  chplx_fork_join_executor = false;

  {
    testForallLoopRanges();
    testForallLoopRanges(42);

    chplx::Sync<int> sy(42);
    testForallLoopRanges(sy);

    chplx::Single<int> si(42);
    testForallLoopRanges(si);
  }

  return hpx::util::report_errors();
}
