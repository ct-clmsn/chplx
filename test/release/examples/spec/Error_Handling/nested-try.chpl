class DemoError : Error { }

proc nestedTry() {
  try {
    try {
      alwaysThrows(0);
    } catch e: DemoError {
      writeln("caught a DemoError");
    }
    writeln("never reached");
  } catch {
    writeln("caught an Error from inner try");
  }
}

   
proc alwaysThrows():int throws {
  throw new owned Error();
  // never reached
  return 1;
}
