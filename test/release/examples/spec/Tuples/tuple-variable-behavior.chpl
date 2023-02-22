record R { var x: int; }

var a: [0..0] int;
var i: int;
var r = new R(0);

// The tuple variable `tup` stores copies of `a`, `i`, and `r`.
var tup = (a, i, r);

a[0] = 1;
i = 2;
r.x = 3;

// This will output (0, 0, (x = 0)).
writeln(tup);
