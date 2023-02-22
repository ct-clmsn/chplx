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
config const option = true;

proc makeRecord() {
  return new R(); // creates a new R record
}

proc elideCopy() {
  var x = makeRecord();
  var y = x; // copy elided because 'x' is not used again
  writeln("block ending");
}
elideCopy();

proc noElideCopy() {
  var x = makeRecord();
  var y = x;  // copy is not elided because 'x' is used again
  writeln(x); // 'x' used here
  writeln("block ending");
}
noElideCopy();

proc elideCopyInReturningConditional() {
  var x = makeRecord();
  if option {
    var y = x; // copy elided because 'x' is not used again
    writeln("returning");
    return;    // because this branch of conditional returns
  }
  writeln(x);  // mention of 'x' here not relevant
  writeln("block ending");
}
elideCopyInReturningConditional();

proc elideCopyBothConditional() {
  var x = makeRecord();
  var y; // split initialization below
  if option {
    y = x;
  } else {
    y = x;
  }
  // copy is elided because 'x' is not used after the copy
  // (in either branch of the conditional or after it)
  writeln("block ending");
}
elideCopyBothConditional();
