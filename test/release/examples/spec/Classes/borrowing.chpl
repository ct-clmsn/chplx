class C { }
proc test() {
  var own = new owned C();   // 'own' manages the memory of the instance
  var b = own.borrow();      // 'b' refers to the same instance but has no
                             // impact on the lifetime.

  var bc: borrowed C = own;  // 'bc' stores the result of own.borrow()
                             // due to coercion from owned C to
                             // borrowed C

                             // Note that these coercions can also apply
                             // in the context of procedure calls.

                             // the instance referred to by 'own' is
                             // deleted here, at the end of the containing
                             // block.
}

   
test();
