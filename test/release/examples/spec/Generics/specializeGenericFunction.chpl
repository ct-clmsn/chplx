proc foo(x) {
  writeln("in generic foo(x)");
}
proc foo(x:int) {
  writeln("in specific foo(x:int)");
}

var myReal:real;
foo(myReal); // outputs "in generic foo(x)"
var myInt:int;
foo(myInt); // outputs "in specific foo(x:int)"

   
