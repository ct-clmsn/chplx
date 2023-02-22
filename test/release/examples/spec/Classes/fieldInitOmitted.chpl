class LabeledPoint {
  var x : real;
  var y : real;
  var msg : string = 'Unlabeled';

  proc init(x: real, y: real) {
    this.x = x;
    this.y = y;
    // compiler inserts "this.msg = 'Unlabeled'";
  }

  proc init(msg : string) {
    // compiler inserts "this.x = 0.0;"
    // compiler inserts "this.y = 0.0;"
    this.msg = msg;
  }
}

var A = new LabeledPoint(2.0, 3.0);
var B = new LabeledPoint("Origin");

   
writeln(A);
writeln(B);
