//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <cstddef>

template <typename T, bool Stridable>
void testDomain(chplx::Domain<1, T, Stridable> const &d) {

  chplx::Domain<1, T, Stridable> d1;
  HPX_TEST((chplx::isDomainType<chplx::Domain<1, T, Stridable>>));
  HPX_TEST(chplx::isDomainValue(d1));

  HPX_TEST_EQ(d, d);
  HPX_TEST_NEQ(d, d1);

  HPX_TEST_EQ(d.stridable(), Stridable);
  HPX_TEST_EQ(d.size(), d.dim(0).size());
  HPX_TEST_EQ(d.shape(), chplx::Tuple(d.dim(0).size()));
  HPX_TEST_EQ(d.lowBound(), chplx::Tuple(d.dim(0).lowBound()));
  HPX_TEST_EQ(d.low(), chplx::Tuple(d.dim(0).low()));
  HPX_TEST_EQ(d.first(), chplx::Tuple(d.dim(0).first()));
  HPX_TEST_EQ(d.highBound(), chplx::Tuple(d.dim(0).highBound()));
  HPX_TEST_EQ(d.high(), chplx::Tuple(d.dim(0).high()));
  HPX_TEST_EQ(d.last(), chplx::Tuple(d.dim(0).last()));
  HPX_TEST_EQ(d.stride(), chplx::Tuple(d.dim(0).stride()));
  HPX_TEST_EQ(d.alignment(), chplx::Tuple(d.dim(0).alignment()));

  HPX_TEST(d.isBounded());
  HPX_TEST(d.isRectangular());
  HPX_TEST(!d.isIrregular());
  HPX_TEST(!d.isAssociative());
  HPX_TEST(!d.isSparse());
}

template <typename T, bool Stridable>
void testDomain(chplx::Domain<2, T, Stridable> const &d) {

  chplx::Domain<2, T, Stridable> d1;
  HPX_TEST((chplx::isDomainType<chplx::Domain<2, T, Stridable>>));
  HPX_TEST(chplx::isDomainValue(d1));

  HPX_TEST_EQ(d, d);
  HPX_TEST_NEQ(d, d1);

  HPX_TEST_EQ(d.stridable(), Stridable);
  HPX_TEST_EQ(d.size(), d.dim(0).size() * d.dim(1).size());
  HPX_TEST_EQ(d.shape(), chplx::Tuple(d.dim(0).size(), d.dim(1).size()));
  HPX_TEST_EQ(d.lowBound(),
              chplx::Tuple(d.dim(0).lowBound(), d.dim(1).lowBound()));
  HPX_TEST_EQ(d.low(), chplx::Tuple(d.dim(0).low(), d.dim(1).low()));
  HPX_TEST_EQ(d.first(), chplx::Tuple(d.dim(0).first(), d.dim(1).first()));
  HPX_TEST_EQ(d.highBound(),
              chplx::Tuple(d.dim(0).highBound(), d.dim(1).highBound()));
  HPX_TEST_EQ(d.high(), chplx::Tuple(d.dim(0).high(), d.dim(1).high()));
  HPX_TEST_EQ(d.last(), chplx::Tuple(d.dim(0).last(), d.dim(1).last()));
  HPX_TEST_EQ(d.stride(), chplx::Tuple(d.dim(0).stride(), d.dim(1).stride()));
  HPX_TEST_EQ(d.alignment(),
              chplx::Tuple(d.dim(0).alignment(), d.dim(1).alignment()));
  HPX_TEST(d.isRectangular());
  HPX_TEST(!d.isIrregular());
  HPX_TEST(!d.isAssociative());
  HPX_TEST(!d.isSparse());
}

namespace detail {

//-----------------------------------------------------------------------------
template <typename... Rs, std::size_t... Is>
void testDomain1D(chplx::Tuple<Rs...> const &r, std::index_sequence<Is...> s) {

  (testDomain(chplx::Domain(std::get<Is>(r))), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, typename... Rs, std::size_t... Js>
void testDomain2D(chplx::Tuple<Rs...> const &r, std::index_sequence<Js...>) {

  (testDomain(chplx::Domain(std::get<Js>(r), std::get<N>(r))), ...);
}

template <typename... Rs, std::size_t... Is>
void testDomain2D(chplx::Tuple<Rs...> const &r, std::index_sequence<Is...> s) {

  (testDomain2D<Is>(r, s), ...);
}

} // namespace detail

template <typename... Rs> void testDomain1D(chplx::Tuple<Rs...> r) {

  detail::testDomain1D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testDomain2D(chplx::Tuple<Rs...> r) {

  detail::testDomain2D(r, std::make_index_sequence<sizeof...(Rs)>());
}

//-----------------------------------------------------------------------------
template <int N, typename T, bool Stridable, typename IndexType>
void testIndexOrder(chplx::Domain<N, T, Stridable> const &d,
                    std::vector<IndexType> const &elements) {

  std::size_t count = 0;
  for (std::size_t i = 0; i != d.size(); ++i) {

    auto idx = d.orderToIndex(i);
    HPX_TEST_EQ(idx, elements[i]);

    std::size_t order = d.indexOrder(idx);
    HPX_TEST_EQ(i, order);
    ++count;
  }
  HPX_TEST_EQ(count, elements.size());
}

namespace detail {

//-----------------------------------------------------------------------------
template <typename... Rs, std::size_t... Is>
void testIndexOrder1D(chplx::Tuple<Rs...> rs, std::index_sequence<Is...> s) {

  auto t = []<typename R>(R r) {
    using domain = chplx::Domain<1, typename R::idxType, r.stridable()>;

    domain d(r);

    std::vector<typename domain::indexType> elements;
    elements.reserve(r.size());
    for (auto val : chplx::iterate(chplx::detail::IteratorGenerator(d))) {
      elements.push_back(val);
    }

    testIndexOrder(d, elements);
  };

  (t(std::get<Is>(rs)), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, typename... Rs, std::size_t... Is>
void testIndexOrder2D(chplx::Tuple<Rs...> rs, std::index_sequence<Is...>) {

  auto t = []<typename R1, typename R2>(R1 r1, R2 r2) {
    using common_type =
        std::common_type_t<typename R1::idxType, typename R2::idxType>;
    using domain =
        chplx::Domain<2, common_type, r1.stridable() || r2.stridable()>;

    domain d(r1, r2);

    std::vector<typename domain::indexType> elements;
    elements.reserve(r1.size() * r2.size());
    for (auto val : chplx::iterate(chplx::detail::IteratorGenerator(d))) {
      elements.push_back(val);
    }

    testIndexOrder(d, elements);
  };

  (t(std::get<Is>(rs), std::get<N>(rs)), ...);
}

template <typename... Rs, std::size_t... Is>
void testIndexOrder2D(chplx::Tuple<Rs...> const &r,
                      std::index_sequence<Is...> s) {

  (testIndexOrder2D<Is>(r, s), ...);
}

//-----------------------------------------------------------------------------
template <std::size_t N, std::size_t M, typename... Rs, std::size_t... Is>
void testIndexOrder3D(chplx::Tuple<Rs...> rs, std::index_sequence<Is...>) {

  auto t = []<typename R1, typename R2, typename R3>(R1 r1, R2 r2, R3 r3) {
    using common_type =
        std::common_type_t<typename R1::idxType, typename R2::idxType,
                           typename R3::idxType>;
    using domain =
        chplx::Domain<3, common_type,
                      r1.stridable() || r2.stridable() || r3.stridable()>;

    domain d(r1, r2, r3);

    std::vector<typename domain::indexType> elements;
    elements.reserve(r1.size() * r2.size() * r3.size());
    for (auto val : chplx::iterate(chplx::detail::IteratorGenerator(d))) {
      elements.push_back(val);
    }

    testIndexOrder(d, elements);
  };

  (t(std::get<Is>(rs), std::get<N>(rs), std::get<M>(rs)), ...);
}

template <std::size_t N, typename... Rs, std::size_t... Is>
void testIndexOrder3D(chplx::Tuple<Rs...> const &r,
                      std::index_sequence<Is...> s) {

  (testIndexOrder3D<Is, N>(r, s), ...);
}

template <typename... Rs, std::size_t... Is>
void testIndexOrder3D(chplx::Tuple<Rs...> const &r,
                      std::index_sequence<Is...> s) {

  (testIndexOrder3D<Is>(r, s), ...);
}

} // namespace detail

template <typename... Rs> void testIndexOrder1D(chplx::Tuple<Rs...> r) {

  detail::testIndexOrder1D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testIndexOrder2D(chplx::Tuple<Rs...> r) {

  detail::testIndexOrder2D(r, std::make_index_sequence<sizeof...(Rs)>());
}

template <typename... Rs> void testIndexOrder3D(chplx::Tuple<Rs...> r) {

  detail::testIndexOrder3D(r, std::make_index_sequence<sizeof...(Rs)>());
}

int main() {

  {
    auto constexpr r1 = chplx::Range(0, 10);
    auto constexpr r2 = chplx::BoundedRange<int, true>(0, 10, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(0, 10, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(0, 10, -2);

    testDomain1D(chplx::Tuple(r1, r2, r3, r4));
    testDomain2D(chplx::Tuple(r1, r2, r3, r4));

    testIndexOrder1D(chplx::Tuple(r1, r2, r3, r4));
    testIndexOrder2D(chplx::Tuple(r1, r2, r3, r4));
    testIndexOrder3D(chplx::Tuple(r1, r2, r3, r4));
  }

  {
    auto constexpr r1 = chplx::Range(1, 9);
    auto constexpr r2 = chplx::BoundedRange<int, true>(1, 9, -1);
    auto constexpr r3 = chplx::BoundedRange<int, true>(1, 9, 2);
    auto constexpr r4 = chplx::BoundedRange<int, true>(1, 9, -2);

    testDomain1D(chplx::Tuple(r1, r2, r3, r4));
    testDomain2D(chplx::Tuple(r1, r2, r3, r4));

    testIndexOrder1D(chplx::Tuple(r1, r2, r3, r4));
    testIndexOrder2D(chplx::Tuple(r1, r2, r3, r4));
    testIndexOrder3D(chplx::Tuple(r1, r2, r3, r4));
  }

  return hpx::util::report_errors();
}
