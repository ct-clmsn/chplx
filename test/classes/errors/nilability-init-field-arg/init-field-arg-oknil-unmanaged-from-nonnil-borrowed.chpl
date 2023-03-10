//  lhs: unmanaged?  rhs: borrowed!  error: mm

class MyClass {  var x: int;  }

var rhs = (new owned MyClass()).borrow();

record MyRecord {
  var lhs: unmanaged MyClass?;
  proc init(in rhs) where ! isSubtype(rhs.type, MyRecord) {
    lhs = rhs;
  }
}

var myr = new MyRecord(rhs);
