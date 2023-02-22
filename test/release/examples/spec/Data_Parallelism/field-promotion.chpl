record Point {
  var x: real;
  var y: real;
}
var A: [1..5] Point = [i in 1..5] new Point(x=i, y=i);
var X = A.x;
A.y = 1.0;

   
writeln(X);
writeln(A);

   
