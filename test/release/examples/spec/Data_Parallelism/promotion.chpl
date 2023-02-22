var A: [1..5] int = [i in 1..5] i;
proc square(x: int) return x**2;
for s in square(A) do writeln(s);

   
