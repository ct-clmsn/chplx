var x: atomic int;
cobegin {
  x.waitFor(1);
  x.write(1);
}
