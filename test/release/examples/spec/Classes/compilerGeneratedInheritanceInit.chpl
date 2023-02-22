class Parent {
  var x, y: real;
}

class Child : Parent {
  var z : real;
}

var A = new Child();
var B = new Child(1.0, 2.0, 3.0); // x=1.0, y=2.0, z=3.0
var C = new Child(y=10.0);

   
writeln(A);
writeln(B);
writeln(C);
