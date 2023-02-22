proc first(t) where isTuple(t) {
  return t(0);
}
proc rest(t) where isTuple(t) {
  proc helper(first, rest...)
    return rest;
  return helper((...t));
}

   
writeln(first((1, 2, 3)));
writeln(rest((1, 2, 3)));

   
