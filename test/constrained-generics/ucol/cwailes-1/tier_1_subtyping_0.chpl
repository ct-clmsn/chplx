class C {
  var data:int;
}

class D : C {}

interface I(T) {
  proc g(x:T):int;
}

proc g(x:C):int {
  return x.data;
}

proc f(x:?T):int where implements I(T) {
  return g(x);
}

// Infers 'implements I(borrowed D);'
// Prints 42
writeln(f((new owned D(42)).borrow()));
