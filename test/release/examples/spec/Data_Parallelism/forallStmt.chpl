config const N = 5;
var a: [1..N] int;
var b = [i in 1..N] i;

   
forall i in 1..N do
  a(i) = b(i);
[i in 1..N] a(i) = b(i);

   
writeln(a);

   
