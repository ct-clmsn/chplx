proc alpha() {
   var a : int;
}

alpha();

proc delta(c : int) : int {
   return c;
}

proc beta(c : int) : int {
   return c;
}

proc phi(c : [] real) {
}

proc chai(c : [] real, d : [] real) {
}

proc betad(b : int) : int {
   return delta(b) + b;
}

var c = 1;
beta(c);
c = beta(c);
c = betad(c);
