record R {
  var i: int;
  var x: real;
  proc print() { writeln("i = ", this.i, ", x = ", this.x); }
}
var A: R;
A.i = 3;
A.print();	// "i = 3, x = 0.0"

var C: R;
A = C;
A.print();	// "i = 0, x = 0.0"

C.x = 3.14;
A.print();	// "i = 0, x = 0.0"

   
