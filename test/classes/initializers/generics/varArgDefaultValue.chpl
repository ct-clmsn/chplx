// This test exercises when a generic value argument to an initializer on a
// generic class with a generic var field has a default value

class Foo {
  var v;

  proc init(vVal = 3) {
    v = vVal;
  }
}

var foo1 = (new owned Foo(true)).borrow(); // specifies a different value (of a different type)
var foo2 = (new owned Foo()).borrow(); // relies on the default value
var foo3 = (new owned Foo(3)).borrow(); // specifies the same value as the default

writeln(foo1);
// We expect v to be 1 in this case, as the default value for the argument
// limits the type of v and vVal to "int", coercing when necessary.
writeln(foo2);
writeln(foo3);
