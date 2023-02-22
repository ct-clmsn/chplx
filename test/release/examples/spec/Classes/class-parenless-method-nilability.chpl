class C {
  proc parenlessParam param {
    return 0;
  }
  proc parenlessType type {
    return this.type;
  }

}

var x: owned C? = new owned C?();
writeln(x.parenlessParam); // prints '0'
writeln(x.parenlessType:string); // prints 'owned C?'
