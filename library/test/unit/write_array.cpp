//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/modules/testing.hpp>

using namespace chplx;

void write1D() {

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3)), 0.0);
    write(strm, arr);
    HPX_TEST_EQ(strm.str(), std::string("0 0 0 0"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(std::move(strm1).str(), std::string("0 0 0 0\n"));
  }

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3)), {1, 2, 3, 4});
    write(strm, arr);
    HPX_TEST_EQ(strm.str(), std::string("1 2 3 4"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(strm1.str(), std::string("1 2 3 4\n"));
  }
}

void write2D() {

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3), Range(0, 2)), 0.0);
    write(strm, arr);
    HPX_TEST_EQ(strm.str(), std::string("0 0 0\n0 0 0\n0 0 0\n0 0 0"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(strm1.str(), std::string("0 0 0\n0 0 0\n0 0 0\n0 0 0\n"));
  }

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3), Range(0, 2)),
              {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}});
    write(strm, arr);
    HPX_TEST_EQ(strm.str(), std::string("1 2 3\n4 5 6\n7 8 9\n10 11 12"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(strm1.str(), std::string("1 2 3\n4 5 6\n7 8 9\n10 11 12\n"));
  }
}

void write3D() {

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3), Range(0, 2), Range(0, 1)), 0.0);
    write(strm, arr);
    HPX_TEST_EQ(strm.str(),
                std::string("0 0\n0 0\n0 0\n\n0 0\n0 0\n0 0\n\n0 0\n0 0\n0 "
                            "0\n\n0 0\n0 0\n0 0"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(strm1.str(),
                std::string("0 0\n0 0\n0 0\n\n0 0\n0 0\n0 0\n\n0 0\n0 0\n0 "
                            "0\n\n0 0\n0 0\n0 0\n"));
  }

  {
    std::stringstream strm;
    Array arr(Domain(Range(0, 3), Range(0, 2), Range(0, 1)),
              {{{0, 1}, {2, 3}, {4, 5}},
               {{6, 7}, {8, 9}, {10, 11}},
               {{12, 13}, {14, 15}, {16, 17}},
               {{18, 19}, {20, 21}, {22, 23}}});
    write(strm, arr);
    HPX_TEST_EQ(strm.str(),
                std::string("0 1\n2 3\n4 5\n\n6 7\n8 9\n10 11\n\n12 13\n14 "
                            "15\n16 17\n\n18 19\n20 21\n22 23"));

    std::stringstream strm1;
    writeln(strm1, arr);
    HPX_TEST_EQ(strm1.str(),
                std::string("0 1\n2 3\n4 5\n\n6 7\n8 9\n10 11\n\n12 13\n14 "
                            "15\n16 17\n\n18 19\n20 21\n22 23\n"));
  }
}

int main() {

  write3D();
  write1D();
  write2D();

  return hpx::util::report_errors();
}
