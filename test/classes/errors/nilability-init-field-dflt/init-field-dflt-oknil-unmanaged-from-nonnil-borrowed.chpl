//  lhs: unmanaged?  rhs: borrowed!  error: mm

class MyClass {  var x: int;  }

var rhs = (new owned MyClass()).borrow();

record MyRecord {
  var lhs: unmanaged MyClass? = rhs;
}

var myr = new MyRecord();

compilerError("done");
