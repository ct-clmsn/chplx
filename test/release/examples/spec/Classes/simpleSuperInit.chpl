class A {
  var a, b : real;

  proc init() {
    this.init(1.0);
  }

  proc init(val : real) {
    this.a = val;
    this.b = val * 2;
  }
}

class B : A {
  var x, y : real;

  proc init(val: real, x: real, y: real) {
    super.init(val);
    this.x = x;
    this.y = y;
  }

  proc init() {
    // implicit super.init();
    this.x = a*2;
    this.y = b*2;
  }
}

var b1 = new B(4.0, 1.0, 2.0);
var b2 = new B();

   
writeln(b1);
writeln(b2);

   
