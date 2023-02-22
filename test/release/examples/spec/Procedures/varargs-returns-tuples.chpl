proc tuple(x ...) return x;

   
writeln(tuple(1));
writeln(tuple("hi", "there"));
writeln(tuple(tuple(1, 2), tuple(3, 4)));

   
