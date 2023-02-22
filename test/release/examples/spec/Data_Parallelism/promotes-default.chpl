var counter: atomic int;

proc nextCounterValue():int {
  var i = counter.fetchAdd(1);
  return i;
}

proc assignCounter(ref x:int, counter=nextCounterValue()) {
  x = counter;
}
var A: [1..5] int;
assignCounter(A);
use Sort;
writeln(sorted(A));

   
