class Point3D {
  var x, y, z : real;

  proc init(x: real, y: real, z: real) {
    this.x = x;
    this.y = y;
    this.z = z;
    // implicit 'this.complete();'
  }

  proc init(u: real) {
    this.init(u, u, u);
    writeln(this);
  }
}

var A = new Point3D(1.0);
