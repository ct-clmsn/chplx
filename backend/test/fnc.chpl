proc alpha() {
   var a : int;
}

alpha();

proc delta(c : int) : int {
   return c;
}

proc beta(b : int) : int {
   return delta(b) + b;
}

var c = 1;
beta(c);
c = beta(c);
