iter bar() {
  yield (1, 1);
  yield (2, 2);
}

for (i,j) in bar() do
  writeln(i+j);
