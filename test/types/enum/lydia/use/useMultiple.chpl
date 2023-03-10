// Verifies that multiple enums can be used on the same line and all their
// symbols are available
module M {
  enum foo {a, b, c};

  enum bar {d, e, f};

  proc main() {
    use foo, bar;
    writeln(a, " ", b, " ", c);
    writeln(d, " ", e, " ", f);
  }
}
