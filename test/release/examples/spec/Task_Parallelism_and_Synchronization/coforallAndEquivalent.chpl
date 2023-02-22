iter iterator() { for i in 1..3 do yield i; }
proc body() { }
coforall i in iterator() {
  body();
}
var runningCount$: sync int = 1;
var finished$: single bool;
for i in iterator() {
  runningCount$.writeEF(runningCount$.readFE() + 1);
  begin {
    body();
    var tmp = runningCount$.readFE();
    runningCount$.writeEF(tmp-1);
    if tmp == 1 then finished$.writeEF(true);
  }
}
var tmp = runningCount$.readFE();
runningCount$.writeEF(tmp-1);
if tmp == 1 then finished$.writeEF(true);
finished$.readFF();
