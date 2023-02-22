record R {
  var i: uint;

  proc hash(): uint {
    writeln("In custom hash function");
    return i;
  }
}

// Creating an associative domain with an 'idxType' of 'R'
// invokes R.hash() as part of its implementation

var D: domain(R);
var r = new R(42);
D += r;
writeln(D);
