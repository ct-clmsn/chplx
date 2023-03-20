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

template <int N, typename T, bool Stridable>
void testForLoopDomain(chplx::Domain<N, T, Stridable> d) {

  std::set<typename chplx::Domain<N, T, Stridable>::indexType> values;

  std::size_t count = 0;

  chplx::forLoop(d, [&](auto value) {
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(d.size()));

  count = 0;
  for (auto const &e : d.these()) {

    ++count;
    HPX_TEST(values.contains(e));
  }

  HPX_TEST_EQ(count, values.size());
}

namespace detail {

//-----------------------------------------------------------------------------
template <typename... Rs, std::size_t... Is>
void testForLoopDomains1D(chplx::Tuple<Rs...> const &r,
                          std::index_sequence<Is...> s) {

  (testForLoopDomain(chplx::Domain(std::get<Is>(r))), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, typename... Rs, std::size_t... Js>
void testForLoopDomains2D(chplx::Tuple<Rs...> const &r,
                          std::index_sequence<Js...>) {

  (testForLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename... Rs, std::size_t... Is>
void testForLoopDomains2D(chplx::Tuple<Rs...> const &r,
                          std::index_sequence<Is...> s) {

  (testForLoopDomains2D<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Js>
void testForLoopDomains3D_2(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Js...>) {

  (testForLoopDomain(
       chplx::Domain(std::get<Js>(r), std::get<N>(r), std::get<M>(r))),
   ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testForLoopDomains3D_1(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForLoopDomains3D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testForLoopDomains3D(chplx::Tuple<Rs...> const &r,
                          std::index_sequence<Is...> s) {

  (testForLoopDomains3D_1<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, std::size_t O, typename... Rs,
          std::size_t... Js>
void testForLoopDomains4D_3(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Js...>) {

  (testForLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r),
                                   std::get<M>(r), std::get<O>(r))),
   ...);
}

template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Is>
void testForLoopDomains4D_2(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForLoopDomains4D_3<N, M, Is>(r, s), ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testForLoopDomains4D_1(chplx::Tuple<Rs...> const &r,
                            std::index_sequence<Is...> s) {

  (testForLoopDomains4D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testForLoopDomains4D(chplx::Tuple<Rs...> const &r,
                          std::index_sequence<Is...> s) {

  (testForLoopDomains4D_1<Is>(r, s), ...);
}

} // namespace detail

template <typename... Rs> void testForLoopDomains1D(chplx::Tuple<Rs...> r) {

  detail::testForLoopDomains1D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForLoopDomains2D(chplx::Tuple<Rs...> r) {

  detail::testForLoopDomains2D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForLoopDomains3D(chplx::Tuple<Rs...> r) {

  detail::testForLoopDomains3D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForLoopDomains4D(chplx::Tuple<Rs...> r) {

  detail::testForLoopDomains4D(r, std::make_index_sequence<sizeof...(Rs)>());
}

int main() {

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testForLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testForLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
