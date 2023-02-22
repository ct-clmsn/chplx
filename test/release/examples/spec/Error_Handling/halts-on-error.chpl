class DemoError : Error { }
proc haltsOnError(i: int): int {
  try! {
    canThrow(i);
  } catch e: DemoError {
    writeln("caught a DemoError");
  }
}
