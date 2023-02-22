class Parent {
  var x, y: real;
}

class Child : Parent {
  var z : real;

  proc init(x: real, y: real, z: real) {
    super.init(x, y);
    this.z = z;
  }

  proc init(z: real) {
    this.init(0.0, 0.0, z);
  }
}

var c = new Child(5.0);

   
writeln(c);
