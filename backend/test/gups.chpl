use Time;
use Math;
use CTypes;

extern proc getenv(name : c_string) : c_string;

proc computeProblemSize(numArrays : int, physicalMemoryBytes : int, memRatio : int, returnLog2 : bool) {
    var totalMem = physicalMemoryBytes;
    var memoryTarget = totalMem / memRatio;
    var numBytesPerType : int = c_sizeof(int) : int;
    var bytesPerIndex :int = numArrays * numBytesPerType;
    var numIndices : int = memoryTarget / bytesPerIndex;

    var lgProblemSize : int = ceil(log2(numIndices:real)) : int;

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
   var ran = 0x2;
   var i :int = ceil(log2(N)) : int;
   var val = 0;
   var J = 0;
   for j in 0..i {
      J = i-j;
      for k in 1..m2count {
          if ((ran >> (k-1)) & 1) then val ^= m2(k-1);
      }
      ran = val;
      if ((N >> J) & 1) then getNextRandom(ran);
   }
   return ran;
}

proc RAStream(vals : [] int, numvals : int, m2 : [] int, m2count : int) {
   var val = getNthRandom(2, m2, m2count);
   for i in 0..numvals {
      val = getNextRandom(val);
      vals[i] = val;
   }
}

param randWidth = 64;
param physicalMemory = 17179869184; //1024;
var memRatio = 4;
var numTables = 1;

var m2 : [0..randWidth] int;
computeM2Values(m2, randWidth);

var N_U = 0;
var n = 0;
n = computeProblemSize(numTables, physicalMemory, memRatio, false);
N_U = n+2;

var z = N_U;
var randval: [0..z] int;

var indexMask = z - 1;
var T : [0..z] int;

for i in 0..z {
  T[i] = i;
}

var timer : stopwatch;

timer.start();
RAStream(randval, z, m2, randWidth);
forall r in 0..z {
   T ( randval ( r ) & indexMask ) ^= randval(r);
}
timer.stop();
writeln(getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string,",", timer.elapsed());
