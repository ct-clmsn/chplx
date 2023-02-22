proc f((x, (y, z))) {
  writeln((x, y, z));
}
proc g(t) where isTuple(t) && t.size == 2 && isTuple(t(1)) && t(1).size == 2 {
  writeln((t(0), t(1)(0), t(1)(1)));
}
f((1, (2, 3)));
g((1, (2, 3)));

   
