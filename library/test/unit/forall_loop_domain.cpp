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

template <int N, typename T, bool Stridable>
void testForallLoopDomain(chplx::Domain<N, T, Stridable> d) {

  using indexType = typename chplx::Domain<N, T, Stridable>::indexType;
  std::set<indexType> values;
  hpx::mutex mtx;

  std::size_t count = 0;

  chplx::forall(d, [&](auto value) {
    std::lock_guard l(mtx);
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
void testForallLoopDomains1D(chplx::Tuple<Rs...> const &r,
                             std::index_sequence<Is...> s) {

  (testForallLoopDomain(chplx::Domain(std::get<Is>(r))), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, typename... Rs, std::size_t... Js>
void testForallLoopDomains2D(chplx::Tuple<Rs...> const &r,
                             std::index_sequence<Js...>) {

  (testForallLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename... Rs, std::size_t... Is>
void testForallLoopDomains2D(chplx::Tuple<Rs...> const &r,
                             std::index_sequence<Is...> s) {

  (testForallLoopDomains2D<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Js>
void testForallLoopDomains3D_2(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Js...>) {

  (testForallLoopDomain(
       chplx::Domain(std::get<Js>(r), std::get<N>(r), std::get<M>(r))),
   ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testForallLoopDomains3D_1(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testForallLoopDomains3D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testForallLoopDomains3D(chplx::Tuple<Rs...> const &r,
                             std::index_sequence<Is...> s) {

  (testForallLoopDomains3D_1<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, std::size_t O, typename... Rs,
          std::size_t... Js>
void testForallLoopDomains4D_3(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Js...>) {

  (testForallLoopDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r),
                                      std::get<M>(r), std::get<O>(r))),
   ...);
}

template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Is>
void testForallLoopDomains4D_2(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testForallLoopDomains4D_3<N, M, Is>(r, s), ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testForallLoopDomains4D_1(chplx::Tuple<Rs...> const &r,
                               std::index_sequence<Is...> s) {

  (testForallLoopDomains4D_2<N, Is>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testForallLoopDomains4D(chplx::Tuple<Rs...> const &r,
                             std::index_sequence<Is...> s) {

  (testForallLoopDomains4D_1<Is>(r, s), ...);
}

} // namespace detail

template <typename... Rs> void testForallLoopDomains1D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopDomains1D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForallLoopDomains2D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopDomains2D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForallLoopDomains3D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopDomains3D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testForallLoopDomains4D(chplx::Tuple<Rs...> r) {

  detail::testForallLoopDomains4D(r, std::make_index_sequence<sizeof...(Rs)>());
}

int main() {
  chplx_fork_join_executor = true;
  exec = new hpx::execution::experimental::fork_join_executor();

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testForallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testForallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  delete exec;

  chplx_fork_join_executor = false;

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testForallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testForallLoopDomains1D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains2D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains3D(chplx::Tuple(r1, r2, r3, r4));
    testForallLoopDomains4D(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
