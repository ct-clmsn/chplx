config const option = false;
proc main() {
  const x;
  if option {
    x = 6;
  } else {
    {
      x = 4;
    }
  }
  writeln(x);
}
