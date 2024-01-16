// test/studies/hpcc/RA/ra-randstream-hpcc06.chpl
// test/studies/hpcc/common/probSize-hpcc06.chpl

use CTypes;
use Time;

extern proc getenv(name : c_string) : c_string;

proc computeProblemSize(numArrays : int, physicalMemoryBytes : int, memRatio : int, returnLog2 : bool) {
    var totalMem : int = physicalMemoryBytes;
    var memoryTarget : int = totalMem / memRatio;
    var numBytesPerType : int = c_sizeof(int) : int;
    var bytesPerIndex : int = numArrays * numBytesPerType;
    var numIndices : int = memoryTarget / bytesPerIndex;

    var lgProblemSize : int = log2(numIndices);

    if (returnLog2) {
      numIndices = 2**lgProblemSize;
      if (numIndices * bytesPerIndex <= memoryTarget) {
        numIndices *= 2;
        lgProblemSize += 1;
      }
    }

   return if returnLog2 then lgProblemSize else numIndices;
}

proc getNextRandom(x : int) : int {
   var poly = 0x7;
   var hirandbit = 0x1 << (64-1);
   return (x << 1) ^ (if (x & hirandbit) then poly else 0);
}

proc computeM2Values(m2 : [] int, count : int) {
   var nextval = 0x1;
   for i in 0..count {
      m2[i] = nextval;
      nextval = getNextRandom(nextval);
      nextval = getNextRandom(nextval);
   }
}

proc getNthRandom(N : int, m2 : [] int, m2count :int) {
   var period = 0x7fffffff/7 : int;

   var n = N % period : int;

   var ran = 0x2;
   if(n <= 0){
      n = 1;
   }
   var i = log2(n);
   var val = 0;
   var J = 0;
   for j in 0..i {
      J = i-j;
      for k in 0..m2count {
          if ((ran >> j) & 1) then val ^= m2(j);
      }
      ran = val;
      if ((n >> i) & 1) then getNextRandom(ran);
   }

   return ran;
}

proc RAStream(vals : [] int, vals_idx : int, numvals : int, start : int, m2 : [] int, m2count : int) {
   var val = getNthRandom(start, m2, m2count);
   var base = vals_idx * numvals;
   var idx = 0;
   for i in 0..numvals {
      idx = base + i;
      idx = idx % numvals;
      val = getNextRandom(val);
      vals[idx] = val;
   }
}

param randWidth = 64;
param physicalMemory = 16437;
var memRatio = 4;
var numTables = 1;

var m2 : [0..randWidth] int;
computeM2Values(m2, randWidth);

var N_U = 0;
var n = 0;
n = computeProblemSize(numTables, physicalMemory, memRatio, true);
N_U = 2**(n+2);

var z = 0;
z = N_U * N_U;
var randval: [0..z] int;

var m = 0;
m = 2**(n);
var indexMask = m - 1;

var T : [0..m] int;

for i in 0..m {
  T[i] = i;
}

var timer : stopwatch;

timer.start();
forall block in 0..N_U {
   RAStream(randval, block, N_U, 0, m2, randWidth);
   for r in 0..N_U {
      T ( (randval ( (block * N_U + r) % z ) & indexMask) % m ) ^= r;
   }
}
timer.stop();

writeln(getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string,",", timer.elapsed());

