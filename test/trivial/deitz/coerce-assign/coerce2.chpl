class C {
  proc foo(i : int, f : real) {
    writeln(i);
    writeln(f);
  }
}

var c = (new owned C()).borrow();
var i = 2;
var j = 3;
c.foo(i, j);
