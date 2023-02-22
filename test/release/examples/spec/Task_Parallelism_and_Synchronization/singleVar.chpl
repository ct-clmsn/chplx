config const n = 44;
proc work(i) {
  // do nothing
}
var count$: sync int = n;  // counter which also serves as a lock
var release$: single bool; // barrier release

forall t in 1..n do begin {
  work(t);
  var myc = count$.readFE();  // read the count, set state to empty
  if myc!=1 {
    write(".");
    count$.writeEF(myc-1);   // update the count, set state to full
    // we could also do some work here before blocking
    release$.readFF();
  } else {
    release$.writeEF(true);  // last one here, release everyone
    writeln("done");
  }
}
