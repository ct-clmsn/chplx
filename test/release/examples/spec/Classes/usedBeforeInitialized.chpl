class Point {
  var x, y : real;

  proc init(x: real, y: real) {
    writeln(this.x); // Error: use of uninitialized field!
    this.x = x;
    this.y = y;
    writeln(this.y);
  }
}
var p = new Point(1.0, 2.0);

   
