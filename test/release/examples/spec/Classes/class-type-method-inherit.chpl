class Parent { }
class Child : Parent { }
proc type Parent.typeMethod() {
  writeln(this:string); // print out 'this', which is a type
}

Child.typeMethod(); // prints 'Parent'
(borrowed Child?).typeMethod(); // prints 'borrowed Parent?'
