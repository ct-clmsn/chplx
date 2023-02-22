module M1 {
  proc foo() {
    writeln("In M1's foo.");
  }
}

module M2 {
  proc main() {
    import M1.foo;

    writeln("In M2's main.");
    foo();
  }
}
