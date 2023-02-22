class ParentClass { }
class ChildClass : ParentClass { }

writeln(isSubtype(ChildClass, ParentClass?)); // outputs true

proc f(type t: ParentClass?) { }
f(ChildClass); // uses implicit subtype conversion

proc g(in arg: ParentClass?) { }
g(new owned ChildClass()); // uses implicit subtype conversion
