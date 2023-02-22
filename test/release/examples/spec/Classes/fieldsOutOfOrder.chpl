class Point3D {
  var x = 1.0;
  var y = 1.0;
  var z = 1.0;

  proc init(x: real) {
    this.x = x;
    // compiler inserts "this.y = 1.0;"
    this.z = y * 2.0;
  }

  proc init(x: real, y: real, z: real) {
    this.x = x;
    this.z = z;
    this.y = y; // Error!
  }
}

var A = new Point3D(1.0);
var B = new Point3D(1.0, 2.0, 3.0);
