canThrow(1); // handling can be omitted; halts if an error occurs

proc throwsErrorsOn() throws {
  // error propagates out of this function
  canThrow(-1);
}

proc doesNotThrowErrorsOn() {
  // causes a halt if called
  alwaysThrows();
}

   
proc canThrow(i: int): int throws {
  if i < 0 then
    throw new owned Error();

  return i + 1;
}
