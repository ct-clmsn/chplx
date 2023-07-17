// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
const N = 10;

var A : [0..N] real;

for i in 0..10 {
   inlinecxx("std::cout << {} << std::endl;", i);
}

forall i in 0..10 {
   inlinecxx("std::cout << {} << std::endl;", i);
}

for i in 0..N {
   inlinecxx("std::cout << {} << std::endl;", i);
}

forall i in 0..N {
   inlinecxx("std::cout << {} << std::endl;", i);
}

for i in 0..N+1 {
   A[i] = 1.0;
   inlinecxx("std::cout << {} << std::endl;", i);
}

forall i in 0..N+1 {
   A[i] = 1.0;
   inlinecxx("std::cout << {} << std::endl;", i);
}

proc alpha() {
   var B : [0..9] real;
   for i in 0..9 {
      B[i] = 1.0;
   }
   forall i in 0..9 {
      B[i] = 1.0;
   }
}

for i in 0..9 do
    A[i] = i;

forall i in 0..9 do
    A[i] = i;

var B : [0..2] int;

coforall tid in 0..2 {
  B[tid] = tid;
}

for i in 0..2 do
   inlinecxx("std::cout << {} << std::endl;", B[i]);

var C : [0..2] int;
coforall tid in 0..2 do
  C[tid] = tid;

for i in 0..2 do
   inlinecxx("std::cout << {} << std::endl;", C[i]);
