proc defaultGeneric(arg) {
  writeln(arg.type:string);
}
class SomeClass { }
var own = new owned SomeClass();
defaultGeneric(own);
writeln(own != nil);

   
