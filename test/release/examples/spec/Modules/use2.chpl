module M1 {
  proc foo() {
    writeln("In M1's foo.");
  }
}

module M2 {
  proc main() {
    use M1;

    writeln("In M2's main.");
    foo();
  }
}
