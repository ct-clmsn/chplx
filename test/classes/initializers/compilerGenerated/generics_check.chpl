// Validate the default initializer created for a generic class with no
// constructor or initializer defined
class GenericClass {
  type t;
  param p = 3;
  var v;
  var nongeneric: int;
}

proc main() {
  var c1 = (new owned GenericClass(bool, 5, 3.0, 2)).borrow();
  writeln(c1.type:string);
  writeln(c1);

  var c2: borrowed GenericClass(int, 2, bool)?;
  writeln(c2.type:string);

  var c3: borrowed GenericClass(real, 5, real) = (new owned GenericClass(real, 5, 2.4, 8)).borrow();
  writeln(c3.type:string);
  writeln(c3);
}
