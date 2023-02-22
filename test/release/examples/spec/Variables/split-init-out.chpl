proc setArgToFive(out arg: int) {
  arg = 5;
}
proc main() {
  var x:int;
  setArgToFive(x); // initializes x
  writeln(x);
}
