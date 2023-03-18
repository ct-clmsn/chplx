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

template <int N, typename T, bool Stridable>
void testCoforallLoopDomain(chplx::Domain<N, T, Stridable> d) {

  using indexType = typename chplx::Domain<N, T, Stridable>::indexType;
  std::set<indexType> values;
  hpx::mutex mtx;

  std::size_t count = 0;

  chplx::coforall(d, [&](auto value) {
    std::lock_guard l(mtx);
    ++count;
    auto p = values.insert(value);
    HPX_TEST(p.second);
  });

  HPX_TEST_EQ(count, static_cast<std::size_t>(d.size()));
  count = 0;

  for (auto val : d.these()) {
    ++count;
    HPX_TEST(values.contains(val));
  }

  HPX_TEST_EQ(count, values.size());
}

namespace detail {

//-----------------------------------------------------------------------------
template <typename... Rs, std::size_t... Is>
void testCoforallLoopDomains1D(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testCoforallLoopDomain(chplx::Domain(std::get<Is>(r))), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, typename... Rs, std::size_t... Js>
void testCoforallLoopDomains2D(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Js...>) {

  (testCoforallLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename... Rs, std::size_t... Is>
void testCoforallLoopDomains2D(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testCoforallLoopDomains2D<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Js>
void testCoforallLoopDomains3D_2(chplx::Tuple<Rs...> const &r,
                                 std::index_sequence<Js...>) {

  (testCoforallLoopDomain(
       chplx::Domain(std::get<Js>(r), std::get<N>(r), std::get<M>(r))),
   ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testCoforallLoopDomains3D_1(chplx::Tuple<Rs...> const &r,
                                 std::index_sequence<Is...> s) {

  (testCoforallLoopDomains3D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testCoforallLoopDomains3D(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testCoforallLoopDomains3D_1<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, std::size_t O, typename... Rs,
          std::size_t... Js>
void testCoforallLoopDomains4D_3(chplx::Tuple<Rs...> const &r,
                                 std::index_sequence<Js...>) {

  (testCoforallLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r),
                                        std::get<M>(r), std::get<O>(r))),
   ...);
}

template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Is>
void testCoforallLoopDomains4D_2(chplx::Tuple<Rs...> const &r,
                                 std::index_sequence<Is...> s) {

  (testCoforallLoopDomains4D_3<N, M, Is>(r, s), ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testCoforallLoopDomains4D_1(chplx::Tuple<Rs...> const &r,
                                 std::index_sequence<Is...> s) {

  (testCoforallLoopDomains4D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testCoforallLoopDomains4D(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testCoforallLoopDomains4D_1<Is>(r, s), ...);
}

} // namespace detail

template <typename... Rs>
void testCoforallLoopDomains1D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopDomains1D(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs>
void testCoforallLoopDomains2D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopDomains2D(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs>
void testCoforallLoopDomains3D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopDomains3D(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs>
void testCoforallLoopDomains4D(chplx::Tuple<Rs...> r) {

  detail::testCoforallLoopDomains4D(r,
                                    std::make_index_sequence<sizeof...(Rs)>());
}

int main() {

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testCoforallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testCoforallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testCoforallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
