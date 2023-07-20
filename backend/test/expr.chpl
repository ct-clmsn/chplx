// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

var a : int = 1 + 1;
a = a + 1;
a = 1;
a = 1 + a;
a = 1 + 1;
a = a;
a = a + a;
a = a + a + a;
a = a + a + 1;
a = 1 + a + a + a;
a = a + 1 + a + a;
a = a + a + 1 + a;
a = a + a + a + 1;
a = 1 + (1 + 1);
a = (1 + 1) + 1;
a = a + ( ( a + a ) + a );
a = ( ( a + a ) + a ) + a;
a = 1 + 1 * 1;
a = (1 + a);
a = (a + a);
a = a + (a + a);
a = (a + a) + a;
a = a + ( ( a + a ) + a );
a = ( ( a + a ) + a ) + a;

proc value() : int {
   return 1;
}

a = value();
a = 1 + value();
a = value() + 1;
a = ( value() + value() );
a = a + ( value() + value() );
a = ( value() + value() ) + a;
a = ( ( value() + value() ) + value() ) + a;
a = a + ( (value() + value()) + value() );

proc valuea(a : int) : int {
   return a;
}

a = valuea(a);
a = valuea(a+1);
a = 1 + valuea(a+1);
a = valuea(a+1) + 1;
a = 1 + valuea(a);
a = valuea(a) + 1;
a = ( valuea(a) + valuea(a) );
a = a + ( valuea(a) + valuea(a) );
a = ( valuea(a) + valuea(a) ) + a;
a = ( ( valuea(a) + valuea(a) ) + valuea(a) ) + a;
a = a + ( (valuea(a) + valuea(a)) + valuea(a) );

var b : [0..10] int;

b[0] = 1;
b[0] = a;
b(0) = a;
b[0] = b[0];
b[0] = b(0);
b(0) = b[0];
b(0) = b(0);

b[0 + 0] = 1;
b(0 + 0) = 1;

b[0 + 0] = a;
b(0 + 0) = a;
b[0 + 0] = b[0];
b(0 + 0) = b[0];
b[0 + 0] = b[0+0];
b(0 + 0) = b(0+0);

var c = [[1,2], [3,4]];

c[0,0] = 0;
c[0,0] = a;
c(0,0) = 0;
c(0,0) = a;
c[0,0] = c[0,0];
c[0,0] = c(0,0);
c(0,0) = c[0,0];
c(0,0) = c(0,0);
c[0 + 0, 0] = 1;
c(0 + 0, 0) = 1;

var d = 1;
d = d + 1;

var e : int;
e = e + 1;

var y = value();
var z = valuea(a);
var w : int = valuea(a);
var u : int = valuea(1);

proc valueb(a : int) : int {
   var x = valuea(a);
   x = valuea(a);
   return x + valuea(a);
}
