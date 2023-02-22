var x, y = 0;

proc doA(param setter, i: int) ref {
  if i < 0 || i > 1 then
    halt("array access out of bounds");

  if setter && i == 1 && x <= 0 then
    halt("cannot assign value to A(1) if A(0) <= 0");

  if i == 0 then
    return x;
  else
    return y;
}
proc A(i: int) ref {
  return doA(true, i);
}
proc A(i: int) {
  return doA(false, i);
}

A(0) = 0;
A(1) = 1; 

   
