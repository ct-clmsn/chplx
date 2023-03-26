// function call scope isn't getting cleared from stack
// when `var b` is accessed during ast traversal in
// programtreebuilder
//
var a : int;

a = 1 + 1;
a = a + 1;
a = 1 + a;
a = a + a;
a = a + a + a;
a = a + a + 1;
a = 1 + a + a + a;
a = a + 1 + a + a;
a = a + a + 1 + a;
a = a + a + a + 1;

a = 1 + (1 + 1);
a = (1 + 1) + 1;
a = a + ( ( a + a ) + a );
a = ( ( a + a ) + a ) + a;
a = 1 + 1 * 1;
a = (1 + a);
a = (a + a);
a = a + (a + a);
a = (a + a) + a;
a = a + ( ( a + a ) + a );
a = ( ( a + a ) + a ) + a;

/*
proc value() : int {
   return 1;
}

a = value();
a = 1 + value();
a = value() + 1;
a = ( value() + value() );
a = a + ( value() + value() );
a = ( value() + value() ) + a;

a = ( ( value() + value() ) + value() ) + a;
a = a + ( (value() + value()) + value() );

proc value() {
   var b = 1;
   b = b + 1;
}

var b : [0..10] int;

b[0] = 1 + value();
*/
