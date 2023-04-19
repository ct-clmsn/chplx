// Copyright (c) 2023 Christopher Taylor
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. *(See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

var a = 1;

if (a) {
inlinecxx("std::cout << a << std::endl;");
}

var b = 0;

if (b == 0) {
inlinecxx("std::cout << b << std::endl;");
}
else if(b == 1) {
inlinecxx("std::cout << b << std::endl;");
}
else {
inlinecxx("std::cout << b << std::endl;");
}

var c = 0;

if(c == 3) {
inlinecxx("std::cout << c << std::endl;");
}
else if(c == 2) {
inlinecxx("std::cout << c << std::endl;");
}
else if(c == 1) {
inlinecxx("std::cout << c << std::endl;");
}

proc alpha() {
   var d = 1;
   if(d) {
      inlinecxx("std::cout << d << std::endl;");
   }
}
