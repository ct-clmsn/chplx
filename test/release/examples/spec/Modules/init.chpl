proc foo() {
    return 1;
}

   
var x = foo();       // executed at module initialization
writeln("Hi!");      // executed at module initialization
proc sayGoodbye {
  writeln("Bye!");   // not executed at module initialization
}

   
