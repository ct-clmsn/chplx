module M1 {
  const x = 1;
  proc main() {
    writeln("M", x, "'s main");
  }
}
 
module M2 {
  use M1;

  const x = 2;
  proc main() {
    M1.main();
    writeln("M", x, "'s main");
  }
}

   
