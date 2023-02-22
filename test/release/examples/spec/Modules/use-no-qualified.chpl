module B {
  var bSymbol = 3;
}

module A {
  use B as _;

  proc main() {
    writeln(bSymbol);
    // writeln(B.bSymbol); // Would not work
  }
}
