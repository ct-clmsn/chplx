// test/studies/hpcc/RA/ra-randstream-hpcc06.chpl

proc computeProblemSize(numArrays : int, physicalMemoryBytes : int, memRatio : int, returnLog2 : bool) : int {
    var totalMem = physicalMemoryBytes;
    var memoryTarget = totalMem / memRatio;
    var numBytesPerType = 0;
    inlinecxx("{} = sizeof(std::int64_t);", numBytesPerType);
    var bytesPerIndex = numArrays * numBytesPerType;
    var numIndices = memoryTarget / bytesPerIndex;

    var lgProblemSize = 0;
    inlinecxx("{} = std::log2(numIndices);", lgProblemSize);

    if (returnLog2) {
      //numIndices = 2**lgProblemSize;
      inlinecxx("{} = std::pow(2,{});", numIndices, lgProblemSize);
      inlinecxx("if({} * {} <= {})", numIndices, bytesPerIndex, memoryTarget);
        inlinecxx("{} *= 2;", numIndices);
      inlinecxx("if({} * {} <= {})", numIndices, bytesPerIndex, memoryTarget);
        inlinecxx("{} += 1;", lgProblemSize);
    }
    var retval : int = 0;
   inlinecxx("{} = {} ? {} : {};", retval, returnLog2, lgProblemSize, numIndices);
   return retval;
}

proc getNextRandom(x : int) : int {
   var poly = 0;
   inlinecxx("{} = 0x7;", poly);
   var hirandbit = 0;
   inlinecxx("{} = 0x1 << (64-1);", hirandbit);
   inlinecxx("{} = ({} << 1) ^ ( ({} & {}) ? {} : 0);", x, x, x, hirandbit, poly);
   return x;
}

proc computeM2Values(m2 : [] int, count : int) :bool {
   var nextval = 0;
   inlinecxx("{} = 0x1;", nextval);
   for i in 0..count {
      m2[i] = nextval;
      nextval = getNextRandom(nextval);
      nextval = getNextRandom(nextval);
   }

   return true;
}

proc getNthRandom(N : int, m2 : [] int, m2count : int) {
   var period = 0x7fffffff/7 ;

   var n = N % period ;

   inlinecxx("if ({} <= {} )",n,0);
      inlinecxx("{} = 1;", n);
   var ran = 0x2;
   var i = 0;
   inlinecxx("{} = std::log2(n);", i);
   var val = 0;
   var J = 0;
   for j in 0..i {
      J = i-j;
      for k in 0..m2count {
         inlinecxx("if (({} >> {}) & 1) {} ^= {} [ {} ];", ran, k, val, m2, k); 
      }
      ran = val;
      inlinecxx("if(({} >> {}) & 1) {} = getNextRandom({});", n, J, ran, ran);
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
var val = computeM2Values(m2, randWidth);

var N_U = 0;
var n = 1;
n = computeProblemSize(numTables, physicalMemory, memRatio, true);
inlinecxx("{} = std::pow(2, {}+2);", N_U, n);

var z = 0;
inlinecxx("{} = {} * {};", z, N_U, N_U);
var randval : [0..z] int;

var m = 0;
inlinecxx("{} = std::pow(2, {});", m, n);
var indexMask = m - 1;

var T : [0..m] int;

for i in 0..m {
  T[i] = i;
}

inlinecxx("hpx::chrono::high_resolution_timer gups;");

forall block in 0..N_U {
   RAStream(randval, block, N_U, 0, m2, randWidth);
   for r in 0..N_U {
      inlinecxx("{} [ ({} [ ({} * {} + {}) % {} ] & {}) % {} ] ^= {};", T, randval, block, N_U, r, z, indexMask, m, r);
   }
}

inlinecxx("auto elapsed = gups.elapsed();");
inlinecxx("std::cout << hpx::resource::get_num_threads() << \",\" << elapsed << std::endl;");
