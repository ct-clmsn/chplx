proc param int.isOdd() param {
  return this & 0x1 == 0x1;
}

proc type int.size() param {
  return 64;
}

param three = 3;
var seven = 7;

writeln(42.isOdd());          // prints false
writeln(three.isOdd());       // prints true
writeln((42+three).isOdd());  // prints true
// writeln(seven.isOdd());    // illegal since 'seven' is not a param

writeln(int.size());          // prints 64
// writeln(42.size());        // illegal since 'size()' is a type method

   
