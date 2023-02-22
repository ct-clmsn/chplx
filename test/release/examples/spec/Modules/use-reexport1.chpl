module C {
  var cSymbol: int;
}

module B {
  public use C;
}

module A {
  proc main() {
    use B;
    writeln(B.C.cSymbol);
    writeln(B.cSymbol);
  }
}
