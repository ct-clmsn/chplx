module M {
  class Foo { }
}
module M2 {
  use M;
  proc Foo.secondary() { writeln("in secondary method"); }
}
module M3 {
  use M only;

  proc main() {
    var x = (new owned M.Foo()).borrow();
    x.secondary(); // Shouldn't be accessible because we didn't include M2
  }
}
