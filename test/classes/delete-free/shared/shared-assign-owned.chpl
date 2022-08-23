class MyClass {
  var x:int;
}


proc test() {
  var o1 = new owned MyClass(1)?;
  var o2 = new owned MyClass(2)?;
  var o3 = new owned MyClass(3)?;

  var s1:shared MyClass? = o1: shared MyClass?;
  var s2 = o2: shared MyClass?;
  var s3:shared MyClass?;
  s3 = o3: shared MyClass?;

  writeln(s1, " ", o1);
  writeln(s2, " ", o2);
  writeln(s3, " ", o3);
}

test();
