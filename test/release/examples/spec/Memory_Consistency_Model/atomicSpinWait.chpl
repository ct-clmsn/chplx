var x: atomic int;
cobegin {
  while x.read() != 1 do ;  // spin wait - monopolizes processor
  x.write(1);
}
