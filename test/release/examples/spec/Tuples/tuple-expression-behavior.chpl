record R { var x: int; }

var a: [0..0] int;
var i: int;
var r: R;

//
// The int `i` is copied when captured into the tuple expression,
// but `a` and `r` are not.
//
test((a, i, r));

// Modify the globals, then print the tuple.
proc test(tup) {
  a[0] = 1;
  i = 2;
  r.x = 3;

  // Outputs (1, 0, (x = 3)).
  writeln(tup);
}
