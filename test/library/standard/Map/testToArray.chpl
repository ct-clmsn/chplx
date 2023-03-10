use Sort;
use Map;

config param parSafe = false;

var m = new map(int, int, parSafe);
for i in -10..10 do
  m.add(i,-i);

var pairA = m.toArray(),
    keysA = m.keysToArray(),
    valsA = m.valuesToArray();

writeln(sorted(pairA));
writeln(sorted(keysA));
writeln(sorted(valsA));
