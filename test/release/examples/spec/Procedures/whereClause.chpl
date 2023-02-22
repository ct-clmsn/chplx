proc foo(x) where x.type == int { writeln("int"); }
proc foo(x) where x.type == real { writeln("real"); }

   
foo(3);
foo(3.14);

   
