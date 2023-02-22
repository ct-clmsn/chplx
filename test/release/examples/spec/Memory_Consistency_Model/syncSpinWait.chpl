if false { // }

   
var x: int;
cobegin with (ref x) {
  while x != 1 do ;  // INCORRECT spin wait
  x = 1;
}

   
// {
}
var x$: sync int;
cobegin {
  while x$.readXX() != 1 do ;  // spin wait
  x$.writeXF(1);
}
