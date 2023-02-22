record MyColor {
  var color: int;
}
proc printMyColor(in mc: MyColor) {
  writeln("my color is ", mc.color);
  mc.color = 6;   // does not affect the caller's record
}
var mc1: MyColor;        // 'color' defaults to 0
var mc2: MyColor = mc1;  // mc1's value is copied into mc2
mc1.color = 3;           // mc1's value is modified
printMyColor(mc2);       // mc2 is not affected by assignment to mc1
printMyColor(mc2);       // ... or by assignment in printMyColor()

proc modifyMyColor(inout mc: MyColor, newcolor: int) {
  mc.color = newcolor;
}
modifyMyColor(mc2, 7);   // mc2 is affected because of the 'inout' intent
printMyColor(mc2);
