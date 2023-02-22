class Parent {
  var x, y : real;
}

class Child : Parent {
  var z : real;

  proc init(cond : bool, val : real) {
    if cond {
      super.init(val, val);
      this.z = val;
      this.complete();
    } else {
      this.init(val, val, val);
    }
  }

  proc init(x: real, y: real, z: real) {
    super.init(x, y);
    this.z = z;
  }
}

var c = new Child(true, 5.0);

   
writeln(c);
  
