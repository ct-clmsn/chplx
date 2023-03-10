// Tests the behavior when listing a containing type in the only list of a used
// module that defines a tertiary method on a forwarded type.
module definesType {
  record Container {
    forwarding var x: OtherType;
  }
  record OtherType {
     var foo: int;
  }
}
module definesTertiary {
  public use definesType;

  proc OtherType.tertiaryMethod() {
    writeln("In forwarded tertiary method");
  }
}
module User {
  use definesType;
  // The module used below defines a tertiary method on `OtherType`, the
  // forwarded type of `Container`.  Because of forwarding, this method is also
  // defined on instances of `Container` in that scope, but we've decided that
  // the limitation clause only applies to the immediate type it is defined on,
  // rather than additionally applying to all types it forwards to.
  use definesTertiary only Container;

  proc main() {
    var x = new Container(new OtherType(2));
    x.tertiaryMethod(); // Thus, this should not resolve
  }
}
