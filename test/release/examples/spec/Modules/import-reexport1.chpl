module C {
  var cSymbol: int;
}

module B {
  public import C;
}

module A {
  proc main() {
    import B;
    writeln(B.C.cSymbol);
  }
}
