class PointDoubleX {
  var x, y : real;

  proc init(x: real, y: real) {
    this.x = x;              // initialization
    writeln("x = ", this.x); // use of initialized field
    this.x = this.x * 2;     // assignment, use of initialized field

    this.y = y;              // initialization
  }
}

var p = new PointDoubleX(1.0, 2.0);

   
writeln(p);
