record R {
  var x: int = 0;
  proc init() {
    this.x = 0;
    writeln("init (default)");
  }
  proc init(arg:int) {
    this.x = arg;
    writeln("init ", arg, " ", arg);
  }
  proc init=(other: R) {
    this.x = other.x;
    writeln("init= ", other.x);
  }
  proc deinit() {
    writeln("deinit ", x);
  }
}
operator R.=(ref lhs:R, rhs:R) {
  writeln("lhs ", lhs.x, " = rhs ", rhs.x);
  lhs.x = rhs.x;
}
temporaryInDeclaration();
temporaryInConstRefDeclaration();
temporaryInStatement();
proc makeRecord() {
  return new R(); // creates a new R record
}
proc f(const ref arg) {
  return new R(); // ignores argument, returns new record
}

proc temporaryInDeclaration() {
  const x = f(makeRecord());
  writeln("block ending");
  // 'x' and the temporary result of 'makeRecord()' are deinited here
}

proc temporaryInConstRefDeclaration() {
  const ref x = f(makeRecord());
  writeln("block ending");
  // 'x' and the temporary result of 'makeRecord()' are deinited here
}

proc temporaryInStatement() {
  f(makeRecord());
  // temporary result of 'f()' and 'makeRecord()' are deinited here
  writeln("block ending");
}
