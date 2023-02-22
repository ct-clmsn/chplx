module M1 {
  use M2.M3;
  use M2;
  writeln("In M1's initializer");
  proc main() {
    writeln("In main");
  }
}

module M2 {
  use M4;
  writeln("In M2's initializer");
  module M3 {
    writeln("In M3's initializer");
  }
}

module M4 {
  writeln("In M4's initializer");
}
