//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

void testAtomicBool() {

  {
    chplx::Atomic<bool> b;
    HPX_TEST(!b.read());

    b.write(true);
    HPX_TEST(b.read());

    bool result = b.exchange(false);
    HPX_TEST(result);
    HPX_TEST(!b.read());
  }

  {
    chplx::Atomic<bool> b(false);
    HPX_TEST(!b.read());

    b.write(true);
    HPX_TEST(b.read());

    bool result = b.exchange(false);
    HPX_TEST(result);
    HPX_TEST(!b.read());
  }
}

template <typename T> void testAtomicArithmetic(T value) {

  {
    chplx::Atomic<T> v;
    HPX_TEST_EQ(v.read(), T());

    v.write(value);
    HPX_TEST_EQ(v.read(), value);

    T result = v.exchange(T());
    HPX_TEST_EQ(result, value);
    HPX_TEST_EQ(v.read(), T());
  }

  {
    auto v = chplx::Atomic<T>(T());
    HPX_TEST_EQ(v.read(), T());

    v.write(value);
    HPX_TEST_EQ(v.read(), value);

    T result = v.exchange(T());
    HPX_TEST_EQ(result, value);
    HPX_TEST_EQ(v.read(), T());
  }
}

template <typename T> 
void testAtomicIntegral(T value, T opwith) {

  {
    auto v = chplx::Atomic<T>(value);
    HPX_TEST_EQ(v.read(), value);

    T result = v.fetchOr(opwith);
    HPX_TEST_EQ(result, value);
    HPX_TEST_EQ(v.read(), value | opwith);
  }

  {
    auto v = chplx::Atomic<T>(value);
    HPX_TEST_EQ(v.read(), value);

    T result = v.fetchAnd(opwith);
    HPX_TEST_EQ(result, value);
    HPX_TEST_EQ(v.read(), value & opwith);
  }

  {
    auto v = chplx::Atomic<T>(value);
    HPX_TEST_EQ(v.read(), value);

    T result = v.fetchXor(opwith);
    HPX_TEST_EQ(result, value);
    HPX_TEST_EQ(v.read(), value ^ opwith);
  }
}

int main() {

  testAtomicBool();
  testAtomicArithmetic(42);
  testAtomicArithmetic(42.0);

  testAtomicIntegral(12345, 23);

  return hpx::util::report_errors();
}
