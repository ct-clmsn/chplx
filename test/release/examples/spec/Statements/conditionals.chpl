proc condtest(x:int) {
  if x > 3 then
    if x > 5 then
      write("A,");
    else
      write("B,");

  if x == 2 then
    writeln("two");
  else if x == 4 then
    writeln("four");
  else
    writeln("other");
}

   
for i in 2..6 do condtest(i);

   
