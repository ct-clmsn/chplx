proc fillTuple(param p: int, x: int) {
  var result: p*int;
  for i in result.indices do
    result(i) = x;
  return result;
}

   
writeln(fillTuple(3,3));

   
