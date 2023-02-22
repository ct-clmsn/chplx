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
  manage wrapper as val do val = 8;
}
manageIntWrapper();
