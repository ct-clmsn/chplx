var A: [1..4] uint;
writeln(A);
var e: uint = 77;

   
A = e;

   
writeln(A);
e = 33;
forall a in A do
  a = e;

   
writeln(A);

   
