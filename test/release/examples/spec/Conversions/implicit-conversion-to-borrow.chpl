class C { }
var c:owned C = new owned C();

proc f(arg: borrowed C) { }
f(c); // equivalent to f(c.borrow())
