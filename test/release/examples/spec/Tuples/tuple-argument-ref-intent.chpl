//
// Because the intent of `tup` is `ref`, only value tuples can be
// passed to `passTupleByRef`.
//
proc passTupleByRef(ref tup) {
  tup = (64, 128);
}

var modTup = (0, 0);

//
// Passing `modTup` to `passTupleByRef` will construct a referential
// tuple where each element refers to an element from `modTup`.
//
passTupleByRef(modTup);

// Should print (64, 128).
writeln(modTup);
