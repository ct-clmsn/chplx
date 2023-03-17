//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

//-----------------------------------------------------------------------------
void testIndexBuffer1() {

  chplx::AssocDomain<int> d;
  auto buffer = d.createIndexBuffer(1000);

  for (int i = 0; i != 10000; ++i) {
    buffer.add(i);
  }

  buffer.commit();

  HPX_TEST_EQ(d.size(), static_cast<std::int64_t>(10000));

  for (int i = 0; i != 10000; ++i) {
    auto idx = d.indexOrder(i);
    HPX_TEST_EQ(idx, i);
  }
}

void testIndexBuffer2() {

  chplx::AssocDomain<int> d;

  {
    auto buffer = d.createIndexBuffer(1000);

    for (int i = 0; i != 10000; ++i) {
      buffer.add(i);
    }
  }

  HPX_TEST_EQ(d.size(), static_cast<std::int64_t>(10000));

  for (int i = 0; i != 10000; ++i) {
    auto idx = d.indexOrder(i);
    HPX_TEST_EQ(idx, i);
  }
}

int main() {

  testIndexBuffer1();
  testIndexBuffer2();
  return hpx::util::report_errors();
}
