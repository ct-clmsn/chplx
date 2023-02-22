proc canThrow(i: int): int throws {
  if i < 0 then
    throw new owned Error();

  return i + 1;
}

proc alwaysThrows():int throws {
  throw new owned Error();
  // never reached
  return 1;
}
