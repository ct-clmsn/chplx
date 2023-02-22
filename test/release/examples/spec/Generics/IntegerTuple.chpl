class IntegerTuple {
  param size: int;
  var data: size*int;
}

   
var x = new unmanaged IntegerTuple(3);
writeln(x.data);
delete x;

   
