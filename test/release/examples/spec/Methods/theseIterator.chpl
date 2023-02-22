class ThreeArray {
  var x1, x2, x3: int;
  iter these() ref {
    yield x1;
    yield x2;
    yield x3;
  }
}

   
var ta = new ThreeArray();
for (i, j) in zip(ta, 1..) do
  i = j;

for i in ta do
  writeln(i);

   
