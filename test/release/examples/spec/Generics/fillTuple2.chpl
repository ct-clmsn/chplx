proc fillTuple(param p, x) {
  var result: p*x.type;
  for i in result.indices do
    result(i) = x;
  return result;
}

   
var x = fillTuple(3, 3.14);
writeln(x);
writeln(x.type:string);

   
