//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/modules/testing.hpp>

#include <cstdint>

int main() {

  {
    chplx::Array<int, chplx::Domain<1>> a1(chplx::Range(1, 5));
    HPX_TEST_EQ(a1.size(), static_cast<std::int64_t>(5));
    HPX_TEST(a1.isBounded());
    HPX_TEST_EQ(a1.dims(), chplx::Tuple(chplx::Range(1, 5)));
    HPX_TEST_EQ(a1.dim(0), chplx::Range(1, 5));
    HPX_TEST_EQ(a1(2), 0);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 0);

    a1(2) = 42;
    HPX_TEST_EQ(a1(2), 42);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 42);

    chplx::Array<int, chplx::Domain<1>> a2(std::vector{1, 2, 3, 4, 5});
    HPX_TEST_EQ(a2.size(), static_cast<std::int64_t>(5));
    HPX_TEST(a2.isBounded());
    HPX_TEST_EQ(a2.dims(), chplx::Tuple(chplx::Range(0, 4)));
    HPX_TEST_EQ(a2.dim(0), chplx::Range(0, 4));
    HPX_TEST_EQ(a2(2), 3);
    HPX_TEST_EQ(a2(chplx::Tuple(2)), 3);

    chplx::Array a3(std::vector{1, 2, 3, 4, 5});
    HPX_TEST_EQ(a3.size(), static_cast<std::int64_t>(5));
    HPX_TEST(a3.isBounded());
    HPX_TEST_EQ(a3.dims(), chplx::Tuple(chplx::Range(0, 4)));
    HPX_TEST_EQ(a3.dim(0), chplx::Range(0, 4));

    chplx::Array<int, chplx::Domain<1>> a4 = {1, 2, 3, 4, 5};
    HPX_TEST_EQ(a4.size(), static_cast<std::int64_t>(5));
    HPX_TEST(a4.isBounded());
    HPX_TEST_EQ(a4.dims(), chplx::Tuple(chplx::Range(0, 4)));
    HPX_TEST_EQ(a4.dim(0), chplx::Range(0, 4));

    chplx::Array a5 = {10, 20, 30, 40, 50};
    HPX_TEST_EQ(a5.size(), static_cast<std::int64_t>(5));
    HPX_TEST(a5.isBounded());
    HPX_TEST_EQ(a5.dims(), chplx::Tuple(chplx::Range(0, 4)));
    HPX_TEST_EQ(a5.dim(0), chplx::Range(0, 4));

    a1 = a5;
    HPX_TEST_EQ(a1(2), 30);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 30);

    a1 = chplx::Range(20, 24);
    HPX_TEST_EQ(a1(2), 22);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 22);

    a1 = chplx::Tuple(1, 2, 3, 4, 5);
    HPX_TEST_EQ(a1(2), 3);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 3);

    a1 = chplx::Domain(chplx::Range(20, 24));
    HPX_TEST_EQ(a1(2), 22);
    HPX_TEST_EQ(a1(chplx::Tuple(2)), 22);
  }

  {
    chplx::Array<int, chplx::Domain<2>> a1(chplx::Range(2, 5),
                                           chplx::Range(1, 4));
    HPX_TEST_EQ(a1.size(), static_cast<std::int64_t>(16));
    HPX_TEST(a1.isBounded());
    HPX_TEST_EQ(a1.dims(),
                chplx::Tuple(chplx::Range(2, 5), chplx::Range(1, 4)));
    HPX_TEST_EQ(a1.dim(0), chplx::Range(2, 5));
    HPX_TEST_EQ(a1.dim(1), chplx::Range(1, 4));
    HPX_TEST_EQ(a1(3, 2), 0);
    HPX_TEST_EQ(a1(chplx::Tuple(3, 2)), 0);

    a1(3, 2) = 42;
    HPX_TEST_EQ(a1(3, 2), 42);
    HPX_TEST_EQ(a1(chplx::Tuple(3, 2)), 42);

    auto data2 = std::vector{std::vector{2, 3, 4, 5}, std::vector{1, 2, 3, 4}};
    chplx::Array<int, chplx::Domain<2>> a2(std::move(data2));
    HPX_TEST_EQ(a2.size(), static_cast<std::int64_t>(8));
    HPX_TEST(a2.isBounded());
    HPX_TEST_EQ(a2.dims(),
                chplx::Tuple(chplx::Range(0, 1), chplx::Range(0, 3)));
    HPX_TEST_EQ(a2.dim(0), chplx::Range(0, 1));
    HPX_TEST_EQ(a2.dim(1), chplx::Range(0, 3));
    HPX_TEST_EQ(a2(1, 2), 3);
    HPX_TEST_EQ(a2(chplx::Tuple(1, 2)), 3);

    auto data3 = std::vector{std::vector{2, 3, 4, 5}, std::vector{1, 2, 3, 4}};
    chplx::Array a3(std::move(data3));
    HPX_TEST_EQ(a3.size(), static_cast<std::int64_t>(8));
    HPX_TEST(a3.isBounded());
    HPX_TEST_EQ(a3.dims(),
                chplx::Tuple(chplx::Range(0, 1), chplx::Range(0, 3)));
    HPX_TEST_EQ(a3.dim(0), chplx::Range(0, 1));
    HPX_TEST_EQ(a3.dim(1), chplx::Range(0, 3));

    chplx::Array<int, chplx::Domain<2>> a4({{2, 3, 4, 5}, {1, 2, 3, 4}});
    HPX_TEST_EQ(a4.size(), static_cast<std::int64_t>(8));
    HPX_TEST(a4.isBounded());
    HPX_TEST_EQ(a4.dims(),
                chplx::Tuple(chplx::Range(0, 1), chplx::Range(0, 3)));
    HPX_TEST_EQ(a4.dim(0), chplx::Range(0, 1));
    HPX_TEST_EQ(a4.dim(1), chplx::Range(0, 3));

    chplx::Array a5({{20, 30, 40, 50}, {10, 20, 30, 40}});
    HPX_TEST_EQ(a5.size(), static_cast<std::int64_t>(8));
    HPX_TEST(a5.isBounded());
    HPX_TEST_EQ(a5.dims(),
                chplx::Tuple(chplx::Range(0, 1), chplx::Range(0, 3)));
    HPX_TEST_EQ(a5.dim(0), chplx::Range(0, 1));
    HPX_TEST_EQ(a5.dim(1), chplx::Range(0, 3));

    a1 = a5;
    HPX_TEST_EQ(a1(3, 2), 20);
    HPX_TEST_EQ(a1(chplx::Tuple(3, 2)), 20);

//    a1 = chplx::Domain(chplx::Range(20, 24), chplx::Range(30, 34));
//    HPX_TEST_EQ(a1(3, 2), 21);
//    HPX_TEST_EQ(a1(chplx::Tuple(3, 2)), 21);
  }

  return hpx::util::report_errors();
}
