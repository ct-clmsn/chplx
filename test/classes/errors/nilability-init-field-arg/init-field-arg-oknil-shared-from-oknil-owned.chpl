//  lhs: shared?  rhs: owned?  error

class MyClass {  var x: int;  }

var rhs: owned MyClass?;

record MyRecord {
  var lhs: shared MyClass?;
  proc init(in rhs) where ! isSubtype(rhs.type, MyRecord) {
    lhs = rhs;
  }
}

var myr = new MyRecord(rhs);


