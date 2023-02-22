config const n = 2;

   
var OuterD: domain(2) = {0..n+1, 0..n+1};
var InnerD: domain(2) = {1..n, 1..n};
var A, B: [OuterD] real;

   
writeln(OuterD);
writeln(InnerD);
B = 1;
A[InnerD] = B[InnerD];

   
writeln(A);
writeln(B);

   
