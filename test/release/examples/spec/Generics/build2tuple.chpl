proc build2Tuple(type t, type tt) {
  var x1: t;
  var x2: tt;
  return (x1, x2);
}
var t2 = build2Tuple(int, string);
t2 = (1, "hello");

   
writeln(t2);

   
