class C {
  param p1: int;
  param p2: int;
}

var c1 = (new owned C(2,1)).borrow();
var c2 = (new owned C(6,2)).borrow();
var c3 = (new owned C(6,3)).borrow();

proc f(c: borrowed C(1+1,?p)...) {
  writeln("p1=2 p2=", p, ": ({p1 = ", c(0).p1, ", p2 = ", c(0).p2, "})");
}

proc f(c: borrowed C(1+2+3,?p)...) {
  write("p1=6 p2=", p, ": ({p1 = ", c(0).p1, ", p2 = ", c(0).p2, "}");
  if c.size > 1 {
    write(", {p1 = ", c(1).p1, ", p2 = ", c(1).p2, "}");
  }
  writeln(")");
}

f(c1);
f(c2);
f(c3,c3);
