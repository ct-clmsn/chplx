class Parent {
  var a, b : real;
  proc postinit() {
    writeln("Parent.postinit: ", a, ", ", b);
  }
}

class Child : Parent {
  var x, y : real;
  proc postinit() {
    // compiler inserts "super.postinit();"
    writeln("Child.postinit: ", x, ", ", y);
  }
}

var c = new Child(1.0, 2.0, 3.0, 4.0);
