var s1, s2: sync int;
proc stmt1() { s1.readFE(); }
proc stmt2() { s2.readFE(); s1.writeEF(1); }
proc stmt3() { s2.writeEF(1); }
cobegin {
  stmt1();
  stmt2();
  stmt3();
}
var s1$, s2$, s3$: single bool;
begin { stmt1(); s1$.writeEF(true); }
begin { stmt2(); s2$.writeEF(true); }
begin { stmt3(); s3$.writeEF(true); }
s1$.readFF(); s2$.readFF(); s3$.readFF();
