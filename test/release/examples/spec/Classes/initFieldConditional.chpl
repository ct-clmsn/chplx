class Point {
  var x, y : real;
  var r, theta : real;

  proc init(polar : bool, val : real) {
    if polar {
      // compiler inserts initialization for fields 'x' and 'y'
      this.r = val;
    } else {
      this.x = val;
      this.y = val;
      // compiler inserts initialization for field 'r'
    }
    // compiler inserts initialization for field 'theta'
  }
}

var A = new Point(true, 5.0);
var B = new Point(false, 1.0);

   
writeln(A);
writeln(B);
  
