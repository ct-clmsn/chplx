module M1 {
  proc foo() {
    writeln("In M1's foo.");
  }
}

module M2 {
  use M1;
  proc main() {
    writeln("In M2's main.");
    M1.foo();
  }
}
