proc g() { writeln("side effect"); return "a string"; }
var a, b = 1.0, c, d:int, e, f = g();
writeln((a,b,c,d,e,f));
