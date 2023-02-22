write("|");
var r3 = 0 .. 10 by -3 align 0;
for i in r3 do
  write(" ", i);			// Produces " 9 6 3 0".
writeln();
write("|");
var r4 = 0 .. 10 by -3 align 1;
for i in r4 do
  write(" ", i);			// Produces " 10 7 4 1".
writeln();
