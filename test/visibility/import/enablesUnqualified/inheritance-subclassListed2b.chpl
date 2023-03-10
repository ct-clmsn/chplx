// Tests the behavior when listing a subclass in the import list when importing
// a module that defines a tertiary method on a grandparent type.

// This version lists the type in an explicit list for the import path (using
// {})
module definesClass {
  class Grandparent {
    var a: int;
  }

  class Parent: Grandparent {
    var b: int;
  }

  class Child: Parent {
    var c: int;
  }
}

module definesTertiary {
  public use definesClass;

  proc Grandparent.tertiaryMethod() {
    writeln("In inherited tertiary method");
  }
}

module User {
  use definesClass;
  // The module used below defines a tertiary method on `Grandparent`, the
  // parent type of `Child`'s parent type.  Because of inheritance, this method
  // is also now defined on instances of `Child` in that scope, but we've
  // decided that the limitation clause only applies to the immediate type it is
  // defined on, rather than applying to all parent types of the type listed.
  import definesTertiary.{Child};

  proc main() {
    var x = new owned Child(2, 3);
    x.tertiaryMethod(); // This means this should not work.
  }
}
