use OS;

proc catchingErrors() throws {
  try {
    alwaysThrows(0);
  } catch {
    writeln("caught an error, unnamed catchall clause");
  }

  try {
    var x = alwaysThrows(-1);
    writeln("never reached");
  } catch e:FileNotFoundError {
    writeln("caught an error, FileNotFoundError type filter matched");
  } catch e {
    writeln("caught an error in a named catchall clause");
  }
}
