//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <cstddef>
#include <string>

template <typename Tuple>
void testPlus(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t + t == expected);
}

template <typename Tuple>
void testMinus(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t - t == expected);
}

template <typename Tuple>
void testMultiplies(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t * t == expected);
}

template <typename Tuple>
void testDivides(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t / t == expected);
}

template <typename Tuple>
void testUnaryMinus(Tuple const& t, Tuple const& expected) {

  HPX_TEST(-t == expected);
}

template <typename Tuple>
void testUnaryNot(Tuple const& t, Tuple const& expected) {

  HPX_TEST(~t == expected);
}

template <typename Tuple1, typename Tuple2>
void testUnaryLogicalNot(Tuple1 const& t, Tuple2 const& expected) {

  HPX_TEST(!t == expected);
}

template <typename Tuple>
void testShiftLeft(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t << t == expected);
}

template <typename Tuple>
void testShiftRight(Tuple const& t, Tuple const& expected) {

  HPX_TEST(t >> t == expected);
}

void testHomogenous() {

  {
    chplx::Tuple<> t;

    HPX_TEST(chplx::isTupleType<decltype(t)>);
    HPX_TEST(chplx::isTupleValue(t));
    HPX_TEST(chplx::isHomogeneousTuple(t));

    HPX_TEST_EQ(t.size(), static_cast<std::size_t>(0));
    HPX_TEST(t.indices() ==
             chplx::Range(0, 0, chplx::BoundsCategoryType::Open));
  }

  {
    chplx::Tuple<int> t(1);

    HPX_TEST(chplx::isTupleType<decltype(t)>);
    HPX_TEST(chplx::isTupleValue(t));
    HPX_TEST(chplx::isHomogeneousTuple(t));

    HPX_TEST_EQ(t.size(), static_cast<std::size_t>(1));
    HPX_TEST(t.indices() ==
             chplx::Range(0, 1, chplx::BoundsCategoryType::Open));

    HPX_TEST_EQ(t[0], 1);
    HPX_TEST_EQ(t(0), 1);
  }

  {
    chplx::Tuple t(1, 2, 3);

    HPX_TEST(chplx::isTupleType<decltype(t)>);
    HPX_TEST(chplx::isTupleValue(t));
    HPX_TEST(chplx::isHomogeneousTuple(t));

    HPX_TEST_EQ(t.size(), static_cast<std::size_t>(3));
    HPX_TEST(t.indices() ==
             chplx::Range(0, 3, chplx::BoundsCategoryType::Open));

    HPX_TEST_EQ(t(0), 1);
    HPX_TEST_EQ(t(1), 2);
    HPX_TEST_EQ(t(2), 3);

    testPlus(t, chplx::Tuple(2, 4, 6));
    testMinus(t, chplx::Tuple(0, 0, 0));
    testMultiplies(t, chplx::Tuple(1, 4, 9));
    testDivides(t, chplx::Tuple(1, 1, 1));
    testUnaryMinus(t, chplx::Tuple(-1, -2, -3));
    testUnaryNot(t, chplx::Tuple(~1, ~2, ~3));
    testUnaryLogicalNot(t, chplx::Tuple(!1, !2, !3));
    testShiftLeft(t, chplx::Tuple(1 << 1, 2 << 2, 3 << 3));
    testShiftRight(t, chplx::Tuple(1 >> 1, 2 >> 2, 3 >> 3));
  }
}

void testNonHomogenous() {

  {
    chplx::Tuple<int, long> t(1, 2L);

    HPX_TEST(chplx::isTupleType<decltype(t)>);
    HPX_TEST(chplx::isTupleValue(t));
    HPX_TEST(!chplx::isHomogeneousTuple(t));

    HPX_TEST_EQ(t.size(), static_cast<std::size_t>(2));
    HPX_TEST(t.indices() ==
             chplx::Range(0, 2, chplx::BoundsCategoryType::Open));

    HPX_TEST_EQ(std::get<0>(t), 1);
    HPX_TEST_EQ(std::get<1>(t), 2L);
  }

  {
    chplx::Tuple<int, long, std::string> t(1, 2L, "3");

    HPX_TEST(chplx::isTupleType<decltype(t)>);
    HPX_TEST(chplx::isTupleValue(t));
    HPX_TEST(!chplx::isHomogeneousTuple(t));

    HPX_TEST_EQ(t.size(), static_cast<std::size_t>(3));
    HPX_TEST(t.indices() ==
             chplx::Range(0, 3, chplx::BoundsCategoryType::Open));

    HPX_TEST_EQ(std::get<0>(t), 1);
    HPX_TEST_EQ(std::get<1>(t), 2L);
    HPX_TEST_EQ(std::get<2>(t), std::string("3"));

    testPlus(t, chplx::Tuple(2, 4L, std::string("33")));
  }
}

int main() {

  testHomogenous();
  testNonHomogenous();

  return hpx::util::report_errors();
}
