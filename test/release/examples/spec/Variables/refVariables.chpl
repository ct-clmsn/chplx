var myInt = 51;
ref refInt = myInt;                   // alias of the previous variable
myInt = 62;
writeln("refInt = ", refInt);
refInt = 73;
writeln("myInt = ", myInt);

var myArr: [1..3] int = 51;
proc arrayElement(i) ref  return myArr[i];
ref refToExpr = arrayElement(3);      // alias to lvalue returned by a function
myArr[3] = 62;
writeln("refToExpr = ", refToExpr);
refToExpr = 73;
writeln("myArr[3] = ", myArr[3]);

const constArr: [1..3] int = 51..53;
const ref myConstRef = constArr[2];   // would be an error without 'const'
writeln("myConstRef = ", myConstRef);
