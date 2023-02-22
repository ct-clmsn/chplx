proc propagatesError() throws {
  // control flow changes if an error was thrown;
  // could be indicated more clearly with try
  canThrow(0);

  try canThrow(0);

  try {
    canThrow(0);
  }

  var x = try canThrow(1);
  writeln(x);

  return try canThrow(0);
}
