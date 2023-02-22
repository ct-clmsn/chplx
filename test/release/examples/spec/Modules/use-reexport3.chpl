module C1 {
  var c1Symbol: int;
}

module C2 {
  var c2Symbol: bool;
}

module C3 {
  var c3Symbol = 3;
}

module B {
  public use C1, C2, C3;
}

module A {
  proc main() {
    use B;
    writeln(B.C1.c1Symbol);
    writeln(B.C2.c2Symbol);
    writeln(B.C3.c3Symbol);

    writeln(B.c1Symbol);
    writeln(B.c2Symbol);
    writeln(B.c3Symbol);
  }
}
