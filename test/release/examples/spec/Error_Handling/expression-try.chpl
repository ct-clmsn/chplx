proc expressionTry(): int throws {
  var x = try canThrow(1);
  writeln(x);

  return try canThrow(0);
}
