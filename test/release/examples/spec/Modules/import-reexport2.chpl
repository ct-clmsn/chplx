module C {
  var cSymbol: int;
}

module B {
  public import C.cSymbol;
}

module A {
  proc main() {
    import B;
    writeln(B.cSymbol);
  }
}
