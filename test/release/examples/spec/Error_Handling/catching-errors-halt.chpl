use OS;

proc catchingErrorsHalt() {
  try! {
    var x = alwaysThrows(-1);
    writeln("never reached");
  } catch e:FileNotFoundError {
    writeln("caught a file not found error");
  }
  // errors other than FileNotFoundError cause a halt
}
