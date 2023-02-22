module MX {
  var x: int = 0;
}

module MY {
  var y: int = 0;
}

use MX, MY;

proc printX() {
  writeln(x);
}

proc printY() {
  writeln(y);
}

   
printX();
printY();

   
