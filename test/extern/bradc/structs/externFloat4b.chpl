extern "struct _float4" record _float4 {
  var x,y,z,w: real(32);
}

var myf4: _float4;

myf4.x = 1.2:real(32);
myf4.y = 3.4:real(32);
myf4.z = 5.6:real(32);
myf4.w = 7.8:real(32);

writeln("myf4 = ", (myf4.x, myf4.y, myf4.z, myf4.w));

writeln("myf4 = ", myf4); // should we create default I/O functions for external types?


var A: [1..5] _float4;

for i in 1..5 {
  A(i).x = i:real(32);
  A(i).y = i:real(32)/(10.0:real(32));
  A(i).z = 1.1:real(32);
  A(i).w = -1.1:real(32);
}

writeln("A is: ", A);
