// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// `inlinecxx` is defined in the compiler
//

/*
inlinecxx("std::cout << \"h\" << std::endl;");
inlinecxx("std::cout << \"{}\" << std::endl;", "h");
*/

proc print(i : real) : real {
//   inlinecxx("std::cout << {} << std::endl;", i);
   return 1.0;
}

/*
proc writeln(args...?k) {
   inlinecxx("std::cout << ");
   for arg in args do
      inlinecxx("{}", a);
   inlinecx("std::endl;");
}
*/

/*
proc get(Arr : [D] real, i : int) : real {
   return inlinecxx("{}[{}]", Arr, i);
}

proc +(dst : ref real, src, real) : real {
   return inlinecxx("{} + {}", dst, src);
}

proc -(dst : ref real, src, real) : real {
   return inlinecxx("{} -= {}", dst, src);
}

proc *(dst : ref real, src, real) : real {
   return inlinecxx("{} *= {}", dst, src);
}

proc /(dst : ref real, src, real) : real {
   return inlinecxx("{} /= {}", dst, src);
}

proc %(dst : ref real, src, real) : real {
   return inlinecxx("{} %= {}", dst, src);
}

proc =(dst : ref real, src, real) {
   inlinecxx("{} = {}", dst, src);
}

proc +=(dst : ref real, src, real) {
   inlinecxx("{} += {}", dst, src);
}

proc -=(dst : ref real, src, real) {
   inlinecxx("{} -= {}", dst, src);
}

proc *=(dst : ref real, src, real) {
   inlinecxx("{} *= {}", dst, src);
}

proc /=(dst : ref real, src, real) {
   inlinecxx("{} /= {}", dst, src);
}

proc %=(dst : ref real, src, real) {
   inlinecxx("{} %= {}", dst, src);
}

proc ==(dst : ref real, src, real) : bool {
   return inlinecxx("{} == {}", dst, src);
}

// cast function
//
proc int(value : real) : int {
   return inlinecxx("static_cast<int>({})", value);
}

// backend should use std::format (#include <format>)...
// example...
//
// (value is a Symbol)
//
// std::cout << std::format("hpx::cout << {} << hpx::endl", *value->identifier) << std::endl;
//
proc writeln(value : real) {
   inlinecxx("hpx::cout << {} << hpx::endl", value);
}

writeln(1.0);

*/
