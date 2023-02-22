write("|");
var r1 = 0 .. 10 by 3 align 0;
for i in r1 do
  write(" ", i);			// Produces " 0 3 6 9".
writeln();
write("|");
var r2 = 0 .. 10 by 3 align 1;
for i in r2 do
  write(" ", i);			// Produces " 1 4 7 10".
writeln();
