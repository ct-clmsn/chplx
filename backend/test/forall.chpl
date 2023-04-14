// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

var A : [0..9] real;

for i in 0..9 {
    A[i] = i;
}

forall i in 0..9 {
   A[i] = 1.0;
}

/*
for i in 0..9 do
    A[i] = i;
*/
