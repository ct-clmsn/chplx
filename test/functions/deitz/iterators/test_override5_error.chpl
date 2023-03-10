class C {
  iter foo(n: int) {
    for i in 1..n do
      yield i;
  }
}

class D: C {
  override iter foo(n: int) {
    for i in 1..n by -1 do
      yield (i,i);
  }
}

var c = (new owned C()).borrow();

for i in c.foo(4) do
  writeln(i);
writeln();

c = (new owned D()).borrow();

for i in c.foo(4) do
  writeln(i);
writeln();
