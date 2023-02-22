class C {
  proc type typeMethod() {
    writeln(this:string); // print out 'this', which is a type
  }
}
(C).typeMethod(); // prints 'C'
(owned C).typeMethod(); // prints 'owned C'
(borrowed C?).typeMethod(); // prints 'borrowed C?'

   
