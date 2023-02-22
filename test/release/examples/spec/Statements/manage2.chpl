record IntWrapper {
  var x: int;
}

proc IntWrapper.enterThis() ref: int {
  writeln('entering');
  writeln(this);
  return this.x;
}

proc IntWrapper.leaveThis(in error: owned Error?) throws {
  if error then throw error;
  writeln('leaving');
  writeln(this);
}

proc manageIntWrapper() {
  var wrapper = new IntWrapper();

  // Here we explicitly declare the resource 'val' as 'var'.
  manage wrapper as var val {
    val = 8;
  }
}
manageIntWrapper();
