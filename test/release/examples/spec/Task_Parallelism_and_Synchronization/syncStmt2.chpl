proc stmt1() { }
proc stmt2() { }
sync {
  begin stmt1();
  begin stmt2();
}
cobegin {
  stmt1();
  stmt2();
}
