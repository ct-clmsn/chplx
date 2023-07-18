// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// `inlinecxx` is defined in the compiler
//

proc print() {
   var j : int;
   var i = 1;
   var k = true;
   var l = 1.0;
   var m : bool;
   var n : real;

   inlinecxx("std::cout << \"a\" << std::endl;");
   inlinecxx("std::cout << i << std::endl;");
   inlinecxx("std::cout << {} << std::endl;", i);
   inlinecxx("std::cout << {} << {} << std::endl;", i, l);
*/
}

/*
var j = 1;

proc writeln(args...?k) {
   inlinecxx("std::cout << ");
   for arg in args do
      inlinecxx("{}", a);
   inlinecx("std::endl;");
}
*/

/*
proc writeln(value : real) {
   inlinecxx("hpx::cout << {} << hpx::endl", value);
}

writeln(1.0);
*/
