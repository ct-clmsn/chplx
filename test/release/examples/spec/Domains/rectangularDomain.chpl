var D : domain(2) = {1..2, 1..7};
var A : [D] int;
for i in D.dim(0) do
  for j in D.dim(1) do
    A[i,j] = 7 * i**2 + j;
writeln(A);
