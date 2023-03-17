//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

//-----------------------------------------------------------------------------
template <typename T> void testAssocDomain(chplx::AssocDomain<T> d) {

  static_assert(std::is_same_v<T, typename chplx::AssocDomain<T>::idxType>);

  chplx::AssocDomain<T> d1;
  HPX_TEST(chplx::isDomainType<chplx::AssocDomain<T>>);
  HPX_TEST(chplx::isDomainValue(d1));

  HPX_TEST_EQ(d, d);
  HPX_TEST_NEQ(d, d1);

  auto size = d.size();
  HPX_TEST_EQ(size, d.values().size());

  HPX_TEST(d.isBounded());
  HPX_TEST(!d.isRectangular());
  HPX_TEST(d.isIrregular());
  HPX_TEST(d.isAssociative());
  HPX_TEST(!d.isSparse());

  d.requestCapacity(2);
  d.add(T());
  HPX_TEST_EQ(size + 1, d.size());

  HPX_TEST(d.contains(T()));

  d.remove(T());
  HPX_TEST_EQ(size, d.size());

  HPX_TEST(!d.contains(T()));

  d.clear();
  HPX_TEST_EQ(static_cast<std::int64_t>(0), d.size());
  HPX_TEST_EQ(d, d1);
}

template <typename... Ts> void testAssocDomains(Ts &&...ts) {

  auto create = [&]<typename T>(T val) {
    chplx::AssocDomain<T> d;
    d.add(val);
    return d;
  };

  (testAssocDomain(create(ts)), ...);
}

int main() {

  testAssocDomains(1, std::string("1"));
  return hpx::util::report_errors();
}
