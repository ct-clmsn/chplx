use assocArrayAPItest;
use IO;

config const testSlice = false;

proc main() {

  // Ask that arrays are output with [1,2] style
  stdout.lock();
  var style = stdout._styleInternal();
  style.array_style = QIO_ARRAY_FORMAT_CHPL:uint(8);
  stdout._set_styleInternal(style);
  stdout.unlock();

  writeln([1,2,3]);

  var D:domain(string);
  var A:[D] real;

  D += "zero";
  D += "pointone";
  D += "half";
  D += "one";
  D += "two";
  D += "three";
  D += "four";
  D += "five";
  D += "six";
  D += "seven";
  D += "eight";
  D += "nine";
  D += "ten";

  if !testSlice {
    testAssocArrayAPI(A);
  } else {
    const DGood = D;
    D += "eleven";
    D += "twelve";
    D += "thirteen";
    D += "gogol";
    D += "infinity";

    testAssocArrayAPI(A[DGood]);
  }
}
