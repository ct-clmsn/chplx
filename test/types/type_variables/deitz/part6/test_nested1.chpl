class C {
  type t;
  var x = 1;
  var y : t;
  proc foo() {
    proc bar(_x) {
      writeln("default bar");
    }
    proc bar(_x : string) {
      writeln("bar of string");
    }
    bar(x);
    bar("hello world");
  }
}

var c = (new owned C(int)).borrow();
c.foo();
writeln(c);
