class Integer {
  var x:int;
}
proc deferInNestedBlock() {
  var i = 1;
  writeln("before inner block");
  {
    var c = new unmanaged Integer(i);
    writeln("created ", c);
    defer {
      writeln("defer action: deleting ", c);
      delete c;
    }
    writeln("in inner block");
    // note, defer action is executed no matter how this block is exited
  }
  writeln("after inner block");
}
deferInNestedBlock();
