record MyNode {
  var field;  // since no type is specified here, MyNode is a generic type
}

proc MyNode.foo() {
  writeln("in generic MyNode.foo()");
}
proc (MyNode(int)).foo() {
  writeln("in specific MyNode(int).foo()");
}

var myRealNode = new MyNode(1.0);
myRealNode.foo(); // outputs "in generic MyNode.foo()"
var myIntNode = new MyNode(1);
myIntNode.foo(); // outputs "in specific MyNode(int).foo()"
