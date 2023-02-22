class C {
  var x: int;
  var y: real = 3.14;
  var z: string = "Hello, World!";
}

   
var c1 = new C();
var c2 = new C(2);
var c3 = new C(z="");
var c4 = new C(2, z="");
var c5 = new C(0, 0.0, "");
writeln((c1, c2, c3, c4, c5));
