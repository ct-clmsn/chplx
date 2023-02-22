record myInteger {
  var intValue: int;
}
operator :(from: myInteger, type toType: int) {
  return from.intValue;
}
var x = new myInteger(1);
var y = x:int;
writeln("x is ", x, " : ", x.type:string);
writeln("y is ", y, " : ", y.type:string);
