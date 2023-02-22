class C { }

writeln(isSubtype(C, C?)); // outputs true
writeln(isSubtype(owned C, owned C?)); // outputs true
