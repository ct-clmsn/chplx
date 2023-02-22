proc stmt1() { write(1); }
proc stmt2() { write(2); }
proc stmt3() { write(3); }
proc stmt4() { write(4); }
var n = 3;
serial {
  begin stmt1();
  cobegin {
    stmt2();
    stmt3();
  }
  coforall i in 1..n do stmt4();
}
stmt1();
{
  stmt2();
  stmt3();
}
for i in 1..n do stmt4();
writeln();
