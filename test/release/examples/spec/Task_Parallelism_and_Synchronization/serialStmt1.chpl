config const lo = 9;
config const hi = 23;
proc work(i) {
  if __primitive("task_get_serial") then
    writeln("serial ", i);
}
proc f(i) {
  serial i<13 {
    cobegin {
      work(i);
      work(i);
    }
  }
}

for i in lo..hi {
  f(i);
}
