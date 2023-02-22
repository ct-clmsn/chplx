use OS;

proc catchingErrorsPropagate() throws {
  try {
    var x = alwaysThrows(-1);
    writeln("never reached");
  } catch e:FileNotFoundError {
    writeln("caught a file not found error");
  }
  // errors other than FileNotFoundError propagate
}
