module A {
  var x: int;
}

module MainMod {
  private use A; // note: 'use A' means the same as 'private use A'
  var x = "hello";

  proc main() {
    writeln(x);
  }
}
