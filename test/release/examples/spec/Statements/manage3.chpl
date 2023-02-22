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
  var wrapper1 = new IntWrapper(1);
  var wrapper2 = new IntWrapper(2);

  // Here we invoke two managers within a single manage statement.
  manage wrapper1 as val1, wrapper2 as val2 {
    val1 *= -1;
    val2 *= -1;
  }
}
manageIntWrapper();
