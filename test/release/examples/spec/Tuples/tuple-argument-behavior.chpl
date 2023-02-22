record R { var x: int; }

var modTup = (0, new R(0));

//
// The argument `tup` of `referentialTupleArg` is a referential tuple
// due to the default argument intent.
//
proc referentialTupleArg(tup) {

  // Modify the module variable `modTup`.
  modTup = (3, new R(6));

  //
  // Should print (0, (x = 6)). Recall that a tuple argument with the
  // default argument intent copies integer elements.
  //
  writeln(tup);

  //
  // When `tup` is passed to `valueTupleArg`, a copy of each element
  // is made because the `valueTup` argument has the `in` intent.
  //
  valueTupleArg(tup);

  // Should still print (0, (x = 6)).
  writeln(tup);
}

// The argument `valueTup` is a value tuple due to the `in` intent.
proc valueTupleArg(in valueTup) {
  valueTup = (64, new R(128));
}

//
// When `modTup` is passed to `referentialTupleArg`, its first
// element is copied while its second element is passed as though
// it were `const ref`.
//
referentialTupleArg(modTup);
