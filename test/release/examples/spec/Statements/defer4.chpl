proc deferControl(condition: bool) {
  if condition {
    defer {
      writeln("Inside if");
    }
  }
  return;
  defer {
    writeln("After return");
  }
}
writeln("Condition: false");
deferControl(false);
writeln("Condition: true");
deferControl(true);
