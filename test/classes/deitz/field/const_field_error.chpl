class C {
  var x: int = 1;
  const y: int = 2;
}

var c = (new owned C()).borrow();
c.x = 3;
c.y = 4;
writeln(c);
