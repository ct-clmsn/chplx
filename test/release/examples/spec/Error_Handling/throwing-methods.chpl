class ThrowingObject {
  proc f() throws {
    throw new owned Error();
  }
}

class SubThrowingObject : ThrowingObject {
  // must be marked throws even though it doesn't throw
  proc f() throws {
    writeln("this version doesn't throw");
  }
}
