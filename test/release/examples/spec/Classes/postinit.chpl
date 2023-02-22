class Point3D {
  var x, y : real;
  var max = 10.0;

  proc postinit() {
    verify();
  }

  proc verify() {
    writeln("(", x, ", ", y, ")");
    if x > max || y > max then
      writeln("  Point out of bounds!");
  }
}

var A = new Point3D();
var B = new Point3D(1.0, 2.0);
var C = new Point3D(y=5.0);
var D = new Point3D(50.0, 50.0);

   
