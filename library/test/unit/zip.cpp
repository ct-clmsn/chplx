//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace detail {

template <typename R>
decltype(auto) collectResult(R &&r, std::size_t max_values) {

  std::vector<typename std::decay_t<R>::indexType> result;
  result.reserve(max_values);
  for (auto elem : r.these()) {
    result.push_back(elem);
    if (--max_values == 0) {
      break;
    }
  }
  return result;
}

template <typename... Rs>
decltype(auto) collectResults(std::size_t max_values, Rs &&...rs) {

  return chplx::Tuple(collectResult(rs, max_values)...);
}

template <typename... Ts1, typename... Ts2>
bool compare(chplx::Tuple<Ts1...> const &t,
             chplx::Tuple<std::vector<Ts2>...> const &expected, std::size_t i) {

  return t == chplx::detail::lift(expected, [i](auto &&v) { return v[i]; });
}

template <typename... Ts>
bool compareSizes(std::size_t size,
                  chplx::Tuple<std::vector<Ts>...> const &expected) {

  auto result = chplx::detail::lift(
      expected, [size](auto &&v) { return v.size() == size; });

  return chplx::reduce(
      result, [](bool init, bool val) { return init && val; }, true);
}

template <typename... Rs> auto collectSizes(Rs const &...rs) {

  return chplx::detail::lift(chplx::Tuple(rs...), [](auto &&r) {
    return r.isBounded() ? r.size() : chplx::detail::MaxValue_v<std::size_t>;
  });
}

} // namespace detail

template <typename... Rs> void testZip(Rs &&...rs) {

  std::size_t max_values = chplx::reduce(
      detail::collectSizes(rs...),
      [](std::size_t init, std::size_t val) { return (std::min)(init, val); },
      chplx::detail::MaxValue_v<std::size_t>);

  auto expected = detail::collectResults(max_values, rs...);

  std::size_t count = 0;

  for (auto &&element : chplx::zip(std::forward<Rs>(rs)...).these()) {
    HPX_TEST(detail::compare(element, expected, count));
    ++count;
  }

  HPX_TEST(detail::compareSizes(count, expected));
}

int main() {

  // zippered iteration with bounded ranges
  {
    auto constexpr r1 = chplx::Range(0, 10);

    testZip(r1);
    testZip(r1, r1);
    testZip(r1, r1, r1);
    testZip(r1, r1, r1, r1);

    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    testZip(r1, r2);
    testZip(r2, r1);

    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);

    testZip(r3);
    testZip(r3, r3);
    testZip(r3, r3, r3);
    testZip(r3, r3, r3, r3);

    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);
    testZip(r3, r4);
    testZip(r4, r3);
  }

  // iteration with bounded ranges of different length
  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::Range(2, 5);
    testZip(r1, r2);
    testZip(r2, r1);
  }

  // iteration with unbounded ranges
  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::Range(1);
    auto constexpr r3 = chplx::Range(chplx::RangeInit::noValue, 9, -1);
    testZip(r1, r2);
    testZip(r1, r3);
    testZip(r2, r1);
    testZip(r3, r1);
  }

  return hpx::util::report_errors();
}
