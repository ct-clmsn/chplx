class Integer {
  var x:int;
}
proc deferInLoop() {
  for i in 1..10 {
    var c = new unmanaged Integer(i);
    writeln("created ", c);
    defer {
      writeln("defer action: deleting ", c);
      delete c;
    }
    writeln(c);
    if i == 2 then
      break;
  }
}
deferInLoop();
