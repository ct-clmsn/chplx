//  lhs: shared?  rhs: owned?  deprecated.

class MyClass {
  var x: int;
}

var rhs: owned MyClass?;

var lhs: shared MyClass? = rhs;


