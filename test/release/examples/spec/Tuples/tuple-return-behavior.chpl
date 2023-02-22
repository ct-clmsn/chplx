record R { var x: int; }
var a: [0..0] int;
var i: int;
var r = new R(0);

//
// The value tuple returned by `returnTuple` is passed to the
// function `updateGlobalsAndOutput`. It is implicitly converted
// into a referential tuple because the formal argument `tup`
// has the default argument intent.
//
updateGlobalsAndOutput(returnTuple());

//
// The function `returnTuple` returns a value tuple that contains
// a copy of the array `a`, the integer `i`, and the record `r`.
//
proc returnTuple() {
  return (a, i, r);
}

proc updateGlobalsAndOutput(tup) {
  a[0] = 1;
  i = 2;
  r.x = 3;

  //
  // Because the tuple passed to `updateGlobalsAndOutput` is a value
  // tuple and contains no references, the assignments made to `a`,
  // `i`, and `r` above are not visible in `tup` when it is printed.
  // This `writeln` will output (0, 0, (x = 0)).
  //
  writeln(tup);
}
