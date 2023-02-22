module C {
  var cSymbol: int;
}

module B {
  public use C as _;
}

module A {
  proc main() {
    use B;
    // writeln(B.C.cSymbol); // Would not work
    writeln(B.cSymbol);
  }
}
