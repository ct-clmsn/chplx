proc haltsOnError():int {
  // the try! next to the throwing call
  // halts the program if an error occurs.
  return try! canThrow(0);
}

proc haltsOnErrorBlock() {
  try! {
    canThrow(1);
    canThrow(0);
  }
}
