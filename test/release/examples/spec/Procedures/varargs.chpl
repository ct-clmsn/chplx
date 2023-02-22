proc mywriteln(xs ...?k) {
  for x in xs do
    writeln(x);
}

   
mywriteln("hi", "there");
mywriteln(1, 2.0, 3, 4.0);

   
