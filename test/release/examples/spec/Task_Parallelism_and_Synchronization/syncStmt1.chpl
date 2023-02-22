config const n = 9;
proc work() {
  write(".");
}
sync for i in 1..n do begin work();
writeln("done");
