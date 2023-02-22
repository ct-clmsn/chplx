class C {
  var x: int;
  var y: string;

  iter type myIter() {
    yield 3;
    yield 5;
    yield 7;
    yield 11;
  }
}

for i in C.myIter() do
  writeln(i);

   
