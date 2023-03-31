proc alpha() {
   var a : int;
}

alpha();

proc beta(b : int) : int {
   return b;
}

var c = 1;
beta(c);

//c = beta(c);

//inlinecxx("std::cout << c << std::endl;");
