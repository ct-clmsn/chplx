// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
var k = [1,2,3,4,5];
var l = [[1,2], [3,4]];
var m = [[1],[2], [3,4,5]];
var n = [[1,2],[1,2]];
var o = [[[1,2],[1,2],[3,4]],[[5,6],[7,8]],[[9,10],[11, 12],[13,14]]];
var p = [[1,2,3,4,5]];
var q = [[[1,2,3,4,5]]];

var u : bool;
var v : real;
var w : byte;
var x : int;
var y : complex;
var z : string;

var a : [1..10] int;
var b : [1..10] real;
var c : [1..10] complex;
var d : [1..10] string;
var e : [1..10] byte;
var f : [1..10] bool;

var g = 1;
var h = true;
var i = "value";
var j = 1.0;

var aa = 1 + 1;

proc dd() {
   return 1;
}

var bb = dd();
var zz = 1;
var aaa : [zz..10] real;
var aaaa : [1..zz] int;
var bbbb : [zz..zz] int;
aaa[zz] = aaa[zz];
aaa(zz) = aaa(zz);

//var a = {1..10};
//var a : domain(1) = {1..10};
