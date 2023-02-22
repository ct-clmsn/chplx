proc myType(x) type {
  if numBits(x.type) <= 32 then return int(32);
  else return int(64);
}

   
var a = 4: int(32),
    b = 4.0;
var at: myType(a),
    bt: myType(b);
writeln((numBits(at.type), numBits(bt.type)));

   
