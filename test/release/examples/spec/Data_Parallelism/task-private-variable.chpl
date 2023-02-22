var cnt: atomic int;                     // count our shadow variables
record R { var id = cnt.fetchAdd(1); }

iter myIter() { yield ""; }              // serial iterator, unused

iter myIter(param tag) where tag == iterKind.standalone {
  for 1..2 do
    yield "before coforall";             // shadow var 0 ("top-level")
  coforall 1..3 do
    for 1..2 do
      yield "inside coforall";           // shadow vars 1..3
  for 1..2 do
    yield "after coforall";              // shadow var 0, again
}

forall str in myIter()
  with (var tpv: R)                      // declare a task-private variable
do
  writeln("shadow var: ", tpv.id, "  yield: ", str);

   
