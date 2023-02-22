/*
record myType {
  var x: int;
}

var v: myType;
writeln(v.x);
*/
record myType {
  // this parentheses-less function supports
  // a field-access syntax
  proc x : int {
    return 0; // compute ``x`` and return it
  }
}

var v: myType;
writeln(v.x);
