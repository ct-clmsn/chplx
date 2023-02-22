{ // }

   
proc foo(x: ?t, y: [?D] t) {
  for i in D do
    y[i] = x;
}

   
// {
var x = 1.5;
var y: [1..4] x.type;
foo(x, y);
writeln(y);
}
{ // }

   
proc foo(x, y: [] x.type) {
  for i in y.domain do
    y[i] = x;
}

   
// {
var x = 1.5;
var y: [1..4] x.type;
foo(x, y);
writeln(y);
}

   
