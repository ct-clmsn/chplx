module A {
  var x: int;
}

module B {
  public use A;
}

module C {
  var x: bool;
}

module MainMod {
  use B, C;

  proc main() {
    writeln(x);
  }
}
