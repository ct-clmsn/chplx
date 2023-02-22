class LabeledPoint {
  var x, y : real;
  var max = 10.0;
  var msg : string = 'Unlabeled';

  proc init(x: real, y: real) {
    this.x = x;
    this.y = y;
    // compiler inserts initialization for 'max' and 'msg'

    this.complete(); // 'this' is now considered to be fully initialized

    this.verify();
    writeln(this);
  }

  proc init(msg : string) {
    // compiler inserts initialization for fields 'x', 'y', and 'max'
    this.msg = msg;

    // Illegal: this.verify();
    // Implicit 'this.complete();'
  }

  proc verify() {
    if x > max || y > max then
      halt("LabeledPoint out of bounds!");
  }
}

var A = new LabeledPoint(1.0, 2.0);
var B = new LabeledPoint("Origin");

   
writeln(B);

   
