proc f(A: [], is...)
  return A(is);

   
var B: [1..5] int;
[i in 1..5] B(i) = i;
var C: [1..5,1..5] int;
[(i,j) in {1..5,1..5}] C(i,j) = i+i*j;
writeln(f(B, 3));
writeln(f(C, 3, 3));

   
