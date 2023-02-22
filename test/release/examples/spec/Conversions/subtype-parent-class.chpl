class ParentClass { }
class ChildClass : ParentClass { }

writeln(isSubtype(ChildClass, ParentClass)); // outputs true
writeln(isSubtype(borrowed ChildClass, borrowed ParentClass)); // outputs true

proc f(type t: ParentClass) { }
f(ChildClass); // implicit subtype conversion

proc g(type t: borrowed ParentClass) { }
g(borrowed ChildClass); // implicit subtype conversion

// The implicit subtype conversion can also apply to non-type arguments:
proc h(in arg: owned ParentClass) { }
h(new owned ChildClass()); // implicit subtype conversion
