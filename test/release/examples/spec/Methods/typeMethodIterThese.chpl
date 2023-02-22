class C {
  var x: int;
  var y: string;

  iter type these() {
    yield 1;
    yield 2;
    yield 4;
    yield 8;
  }
}

for i in C do
  writeln(i);

   
