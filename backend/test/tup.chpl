// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
var a = (1, 2);
var b = a(1);
b = a(0);
var c : int = a(0);
var d : int = 1;
var e = 1;
var z : (int, int) = (1, 1);
var f : (int, int);
f(0) = 1;
f(0) = b;
f(1) = f(1);

/*
f = (1, 2.0);
f = (b, b);

for a in f {
   f(0) = a; 
}

var (i, j) = f;
var k : int;
var l : real;
(k, l) = (1, 1.0);
f = (k, l);
*/
