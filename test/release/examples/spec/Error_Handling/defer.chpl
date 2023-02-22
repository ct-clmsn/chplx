proc deferredDelete(i: int) {
  try {
    var huge = allocateLargeObject();
    defer {
      delete huge;
      writeln("huge has been deleted");
    }

    canThrow(i);
    processObject(huge);
  } catch {
    writeln("no memory leaks");
  }
}
