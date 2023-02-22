module MX {
  var x: string = "Module MX";
  proc printX() {
    writeln(x);
  }
}

module MY {
  var y: string = "Module MY";
  proc printY() {
    writeln(y);
  }
}

   
module Test {
  proc main() {
    use MX;
    use MY;
    MX.printX();
    MY.printY();
  }
}

   
