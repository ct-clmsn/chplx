use myrecord;

proc make(i:int) {
  var ret: R;
  ret.setup(x = i);
  ret.verify();
  return ret;
}


iter myiter() {
  yield make(1);
  yield make(2);
}

proc run() {
  var i = 1;
  for r in myiter() {
    assert(r.x == i);
    if i == 1 then break;
    i += 1;
  }
}

run();
verify();
