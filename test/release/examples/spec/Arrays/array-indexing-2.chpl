var A: [1..5, 1..5] real;
var ij: 2*int = (1, 1);
A(ij) = 1.1;
A((1, 2)) = 1.2;
A(1, 3) = 1.3;
A[ij] = -1.1;
A[(1, 4)] = 1.4;
A[1, 5] = 1.5;

   
writeln(ij);
writeln(A);

   
