proc getNextRandom(x : int) : int {
   var poly = 0;
   inlinecxx("{} = 0x7;", poly);
   var hirandbit = 0;
   inlinecxx("{} = 0x1 << (64-1);", hirandbit);
   inlinecxx("{} = ({} << 1) ^ ( ({} & {}) ? {} : 0);", x, x, x, hirandbit, poly);
   return x;
}

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
   var ran = 0;
   inlinecxx("{} = 0x2;", ran);
   var i = 0;
   inlinecxx("{} = std::ceil(std::log2(static_cast<double>({})));", i, N);
   var val = 0;
   var J = 0;
   for j in 0..i {
      J = i-j;
      for k in 1..m2count {
         inlinecxx("if (({} >> ({}-1)) & 1) {} ^= {} [ ({}-1) ];", ran, k, val, m2, k); 
      }
      ran = val;
      inlinecxx("if(({} >> {}) & 1) {} = getNextRandom({});", N, J, ran, ran);
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
config var physicalMemory = 134217728;
config var memRatio = 4;
var memoryTarget = physicalMemory / memRatio;
var bytesPerType = 4;
inlinecxx("{} = sizeof(decltype(std::int64_t));",bytesPerType);
param numTables = 1;

var m2 : [0..randWidth] int;
var val = computeM2Values(m2, randWidth);

var SZ = memoryTarget / bytesPerType;
var N_U = 0;
var n = 1;
n = computeProblemSize(numTables, physicalMemory, memRatio, false);
inlinecxx("{} = {}+2;", N_U, n);

var z : int;
z = N_U;
var randval : [0..z] int;

var indexMask = z - 1;

var T : [0..SZ] int;

for i in 0..SZ {
  T[i] = i;
}

inlinecxx("hpx::chrono::high_resolution_timer gups;");

RAStream(randval, z, m2, randWidth);
forall r in 0..z {
   inlinecxx("{} [ {} [ {} ] & {} ] ^= {} [ {} ];", T, randval, r, indexMask, randval, r);
}

inlinecxx("auto elapsed = gups.elapsed();");
inlinecxx("auto nprocs = hpx::resource::get_num_threads();");
inlinecxx("auto t0 = elapsed / ((double)nprocs);");
inlinecxx("auto nupdates = {} * nprocs * 1;", z);
inlinecxx("auto gups_val = ((double)nupdates) / (t0 / 1.0e9);");
inlinecxx("std::cout << hpx::resource::get_num_threads() << \",\" << elapsed << \",\" << gups_val << \",\" << physicalMemory << \",\" << memRatio << \",\" << n << std::endl;");
