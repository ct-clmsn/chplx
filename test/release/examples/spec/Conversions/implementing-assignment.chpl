record myInteger {
  var intValue: int;
}
operator =(ref lhs: myInteger, rhs: int) {
  lhs.intValue = rhs;
}
proc myInteger.init=(rhs: int) {
  this.intValue = rhs;
}
operator :(from: int, type toType: myInteger) {
  var tmp: myInteger = from; // invoke the init= above
  return tmp;
}
var a = 1:myInteger;  // cast -- invokes operator :

var b: myInteger = 2; // initialization -- invokes init=

var c: myInteger;
c = 3;                // split-initialization -- invokes init=

var d = new myInteger();
d = 4;                // assignment -- invokes operator =
writeln("a is ", a, " : ", a.type:string);
writeln("b is ", b, " : ", b.type:string);
writeln("c is ", c, " : ", c.type:string);
writeln("d is ", d, " : ", d.type:string);
