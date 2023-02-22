config const n = 10;
const D = {1..n};
var A: [D] int = [i in D] i % 7;
proc foo(x) return x % 7;

   
var (minA, minALoc) = minloc reduce zip(A, A.domain); 
var (maxVal, maxValNum) = maxloc reduce zip([i in 1..n] foo(i), 1..n);

   
writeln((minA, minALoc));
writeln((maxVal, maxValNum));

   
