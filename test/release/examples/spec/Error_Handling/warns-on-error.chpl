proc warnsOnError(i: int): int {
  try {
    alwaysThrows(i);
  } catch e {
    writeln("Warning: caught a error ", e);
  }
}
