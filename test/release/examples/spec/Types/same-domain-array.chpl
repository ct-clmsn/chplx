proc makeAnotherArray(arr: []) {
  var newArray: arr.type;
  return newArray;
}
proc equivalentAlternative(arr: []) {
  var newArray:[arr.domain] arr.eltType;
  return newArray;
}
var A:[1..4] int = 1..4;
var B = makeAnotherArray(A);
var C = equivalentAlternative(A);
writeln("A.domain ", A.domain);
writeln("A ", A);
writeln("B.domain ", B.domain);
writeln("B ", B);
writeln("C.domain ", C.domain);
writeln("C ", C);
