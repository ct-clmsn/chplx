class C {
  var i: int;
  proc deinit() { writeln("Bye, bye ", i); }
}

var c : unmanaged C? = nil;
delete c;              // Does nothing: c is nil.

c = new unmanaged C(1); // Creates a new instance.
delete c;               // Deletes that instance: Writes out "Bye, bye 1"
                        // and reclaims the memory that was held by c.
{
  var own = new owned C(2); // Creates a new owned instance
                            // The instance is automatically deleted at
                            // the end of this block, so "Bye, bye 2" is
                            // output and then the memory is reclaimed.
}

   
