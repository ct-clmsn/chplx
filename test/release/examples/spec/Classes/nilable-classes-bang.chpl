class C {
  proc method() { }
}
var c: owned C? = new C();

// Invoke c.method() only when c is non-nil.
if c != nil {
  c!.method(); // c! converts from 'owned C?' to 'borrowed C'
}
