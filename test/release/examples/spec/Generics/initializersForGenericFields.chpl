class MyGenericClass {
  type t1;
  param p1;
  const c1;
  var v1;
  var x1: t1; // this field is not generic

  type t5 = real;
  param p5 = "a string";
  const c5 = 5.5;
  var v5 = 555;
  var x5: t5; // this field is not generic

  proc init(c1, v1, type t1, param p1) {
    this.t1 = t1;
    this.p1 = p1;
    this.c1 = c1;
    this.v1 = v1;
    // compiler inserts initialization for remaining fields
  }
  proc init(type t5, param p5, c5, v5, x5,
            type t1, param p1, c1, v1, x1) {
    this.t1 = t1;
    this.p1 = p1;
    this.c1 = c1;
    this.v1 = v1;
    this.x1 = x1;
    this.t5 = t5;
    this.p5 = p5;
    this.c5 = c5;
    this.v5 = v5;
    this.x5 = x5;
  }
}  // class MyGenericClass

var g1 = new MyGenericClass(11, 111, int, 1);
var g2 = new MyGenericClass(int, "this is g2", 3.3, 333, 3333,
                            real, 2, 222, 222.2, 22);

   
writeln(g1.p1);
writeln(g1.p5);
writeln(g1);
writeln(g2.p1);
writeln(g2.p5);
writeln(g2);

   
