proc addOne(x:int) {
  return x + 1;
}
iter firstN(n:int) {
  for i in 1..n {
    yield i;
  }
}
for number in addOne(firstN(10)) {
  writeln(number);
}

   
