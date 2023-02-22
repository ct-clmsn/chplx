var r1 = 1..10 by -2 # -3;
var r2 = ..6 by -2 # 3;
var r3 = -6..6 by -2 # 3;
var r4 = 1..#6 by -2;

   
writeln(r1 == r2 && r2 == r3 && r3 == r4);
writeln((r1, r2, r3, r4));

   
