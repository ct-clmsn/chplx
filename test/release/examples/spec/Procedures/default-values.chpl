proc foo(x: int = 5, y: int = 7) { writeln(x); writeln(y); }

foo();
foo(7);
foo(y=5);
