class C { }

   
// assume that C is a class
var a:owned C? = new owned C();
var b:owned C?; // default initialized to store `nil`
b = a; // transfers ownership from a to b
writeln(a); // a is left storing `nil`

   
