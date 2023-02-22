iter squares(n: int): int {
  for i in 1..n do
    yield i * i;
}
var A = squares(5);

   
writeln(A);
