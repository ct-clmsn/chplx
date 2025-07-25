// This program file was generated by the chplx compiler.
// The original Chapel program file can be found here: gups_cpp.chpl
//
#include "gups_cpp.hpp"

namespace gups_cpp {

    struct __thisModule *__this = nullptr;

    
        auto __thisModule::getNextRandom(std::int64_t x) {
                std::int64_t poly = 0;
                poly = 0x7;
                std::int64_t hirandbit = 0;
                hirandbit = 0x1 << (64-1);
                x = (x << 1) ^ ( (x & hirandbit) ? poly : 0);
                return x;
    };

        auto __thisModule::computeProblemSize(std::int64_t numArrays,std::int64_t physicalMemoryBytes,std::int64_t memRatio,bool returnLog2) {
                auto totalMem = physicalMemoryBytes;
                auto memoryTarget = totalMem / memRatio;
                std::int64_t numBytesPerType = 0;
                numBytesPerType = sizeof(std::int64_t);
                auto bytesPerIndex = numArrays * numBytesPerType;
                auto numIndices = memoryTarget / bytesPerIndex;
                std::int64_t lgProblemSize = 0;
                lgProblemSize = std::log2(numIndices);
                if(returnLog2){
                        numIndices = std::pow(2,lgProblemSize);
                        if(numIndices * bytesPerIndex <= memoryTarget)
                        numIndices *= 2;
                        if(numIndices * bytesPerIndex <= memoryTarget)
                        lgProblemSize += 1;
        }
                std::int64_t retval = 0;
                retval = returnLog2 ? lgProblemSize : numIndices;
                return retval;
    };

        auto __thisModule::computeM2Values(chplx::Array<std::int64_t, chplx::Domain<1> > & m2,std::int64_t count) {
                std::int64_t nextval = 0;
                nextval = 0x1;
                chplx::forLoop(chplx::Range{0,count}, [&](auto i) {
                        m2(i) = nextval;
                        nextval = getNextRandom(nextval);
                        nextval = getNextRandom(nextval);
        });
                return true;
    };

        auto __thisModule::getNthRandom(std::int64_t N,chplx::Array<std::int64_t, chplx::Domain<1> > & m2,std::int64_t m2count) {
                std::int64_t ran = 0;
                ran = 0x2;
                std::int64_t i = 0;
                i = std::ceil(std::log2(static_cast<double>(N)));
                std::int64_t val = 0;
                std::int64_t J = 0;
                chplx::forLoop(chplx::Range{0,i}, [&](auto j) {
                        J = i - j;
                        chplx::forLoop(chplx::Range{1,m2count}, [&](auto k) {
                                if ((ran >> (k-1)) & 1) val ^= m2 [ (k-1) ];
            });
                        ran = val;
                        if((N >> J) & 1) ran = getNextRandom(ran);
        });
                return ran;
    };

        void __thisModule::RAStream(chplx::Array<std::int64_t, chplx::Domain<1> > & vals,std::int64_t numvals,chplx::Array<std::int64_t, chplx::Domain<1> > & m2,std::int64_t m2count) {
                auto val = getNthRandom(2, m2, m2count);
                chplx::forLoop(chplx::Range{0,numvals}, [&](auto i) {
                        val = getNextRandom(val);
                        vals(i) = val;
        });
    };

        std::int64_t memRatio = 4;

    void __thisModule::__main() {

        const std::int64_t randWidth = 64;
        std::int64_t physicalMemory = 0;
        physicalMemory = 17179869184l;
        const std::int64_t numTables = 1;
        chplx::Array<std::int64_t, chplx::Domain<1> > m2(chplx::Range(0, randWidth));
        auto val = computeM2Values(m2, randWidth);
        std::int64_t SZ = 134217728;
        std::int64_t N_U = 0;
        std::int64_t n = 1;
        n = computeProblemSize(numTables, physicalMemory, memRatio, false);
        N_U = n+2;
        std::int64_t z;
        z = N_U;
        chplx::Array<std::int64_t, chplx::Domain<1> > randval(chplx::Range(0, z));
        auto indexMask = z - 1;
        chplx::Array<std::int64_t, chplx::Domain<1> > T(chplx::Range(0, SZ));
        chplx::forLoop(chplx::Range{0,SZ}, [&](auto i) {
                T(i) = i;
    });
        hpx::chrono::high_resolution_timer gups;
        RAStream(randval, z, m2, randWidth);
        chplx::forall(chplx::Range{0,z}, [&](auto r) {
                T [ randval [ r ] & indexMask ] ^= randval [ r ];
    });
        auto elapsed = gups.elapsed();
        auto nprocs = hpx::resource::get_num_threads();
        auto t0 = elapsed / ((double)nprocs);
        auto nupdates = z * nprocs * 1;
        auto gups_val = ((double)nupdates) / (t0 / 1.0e9);
        std::cout << hpx::resource::get_num_threads() << "," << elapsed << "," << gups_val << "," << physicalMemory << "," << memRatio << "," << n << std::endl;


    }

} // end namespace