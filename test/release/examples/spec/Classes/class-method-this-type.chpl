class C {
  proc primaryMethod() {
    assert(this.type == borrowed C);
  }
}
proc C.secondaryMethod() {
  assert(this.type == borrowed C);
}
proc (owned C?).secondaryMethodWithTypeExpression() {
  assert(this.type == owned C?);
}

var x:owned C? = new owned C();
x!.primaryMethod();   // within the method, this: borrowed C
x!.secondaryMethod(); // within the method, this: borrowed C
x.secondaryMethodWithTypeExpression(); // within the method, this: owned C?
