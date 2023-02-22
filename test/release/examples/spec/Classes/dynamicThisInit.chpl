class Parent {
  var x, y : real;

  proc foo() {
    writeln("Parent.foo");
  }
}

class Child : Parent {
  var z : real;

  proc init(x: real, y: real, z: real) {
    super.init(x, y); // parent's compiler-generated initializer
    foo(); // Parent.foo()
    this.z = z;
    this.complete();
    foo(); // Child.foo()
  }

  override proc foo() {
    writeln("Child.foo");
  }
}

var c = new Child(1.0, 2.0, 3.0);

   
writeln(c);
