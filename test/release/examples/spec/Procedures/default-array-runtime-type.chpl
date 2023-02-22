var D = {1..4};
proc createArrayOverD() {
  var A:[D] int;
  return A;
}

proc bar(arg = createArrayOverD()) {
  writeln(arg.domain);
}

bar(); // arg uses the default, so outputs {1..4}

var B:[0..2] int;
bar(B); // arg refers to B and so has the runtime type from B
        // so outputs {0..2}
