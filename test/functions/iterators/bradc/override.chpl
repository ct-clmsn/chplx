class C {
  iter these() {
    yield 1;
    yield 2;
    yield 3;
  }
}

class D : C {
  override iter these() {
    yield 3;
    yield 2;
    yield 1;
  }
}

var c1: borrowed C = (new owned C()).borrow();
var c2: borrowed C = (new owned D()).borrow();
var d : borrowed D = (new owned D()).borrow();

for i in c1 {
  writeln(i);
}
writeln();

for i in c2 {
  writeln(i);
}
writeln();

for i in d {
  writeln(i);
}
writeln();
