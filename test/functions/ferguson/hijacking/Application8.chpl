  module LibraryZ2 {
    class Base {
      proc run(x:int) {
        writeln("starting LibraryZ.Base.run!");
      }
    }
  }
  module Application8 {
    use LibraryZ2;

    class Widget : Base {
      proc run(x:real) {
        writeln("In Application.Widget.foo");
      }
    }

    proc main() {
      var instance = (new owned Widget()).borrow();
      var x = 1;
      instance.run(x);
    }
  }
