class ThreeArray {
  var x1, x2, x3: int;
  proc this(i: int) ref {
    select i {
      when 1 do return x1;
      when 2 do return x2;
      when 3 do return x3;
    }
    halt("ThreeArray index out of bounds: ", i);
  }
}

   
var ta = new ThreeArray();
ta(1) = 1;
ta(2) = 2;
ta(3) = 3;
for i in 1..3 do
  writeln(ta(i));
ta(4) = 4;

   
