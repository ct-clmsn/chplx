class C { }
var c : owned C?;    // c has class type owned C?, meaning
                     // the instance can be nil and is deleted automatically
                     // when it is not.
c = new C();         // Now c refers to an initialized instance of type C.
var c2 = c.borrow(); // The type of c2 is borrowed C?.
                     // c2 refers to the same object as c.
class D : C {}    // Class D is derived from C.
c = new D();      // Now c refers to an object of type D.
                  // Since c is owned, the previous is deleted.
// the C and D instances allocated above will be reclaimed
// at the end of this block.
