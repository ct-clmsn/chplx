class foo {
  var a : int;
}

proc bar(x : borrowed foo) ref {
  return x;
}

var f : borrowed foo = (new owned foo(a = 12)).borrow();

writeln(f);

var i : int;

writeln(i);

i = bar(f).a;

writeln(i);

bar(f).a = 2;

writeln(f);
