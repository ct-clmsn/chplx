// This program file was generated by the chplx compiler.
// The original Chapel program file can be found here: stream_cpp_add.chpl
//
#include "stream_cpp_add.hpp"

namespace stream_cpp_add {

    struct __thisModule *__this = nullptr;

    
        auto __thisModule::randindex() {
                std::int64_t ret = 0;
                ret = rand()%5;
                return ret;
    };

        void __thisModule::run(chplx::Array<double, chplx::Domain<1> > & A,chplx::Array<double, chplx::Domain<1> > & B,chplx::Array<double, chplx::Domain<1> > & C) {
                hpx::chrono::high_resolution_timer cpy;
                chplx::forall(chplx::Range{1,arraySize}, [&](auto i) {
                        C(i) = A(i) + B(i);
        });
                auto elapsed = cpy.elapsed();
                std::cout << hpx::resource::get_num_threads() << "," << elapsed << "\n";
    };

        std::int64_t arraySize = 134217728;

    void __thisModule::__main() {

        chplx::Array<double, chplx::Domain<1> > A(chplx::Range(1, arraySize));
        chplx::Array<double, chplx::Domain<1> > B(chplx::Range(1, arraySize));
        chplx::Array<double, chplx::Domain<1> > C(chplx::Range(1, arraySize));
        chplx::forall(chplx::Range{1,arraySize}, [&](auto i) {
                A(i) = randindex();
                B(i) = randindex();
                C(i) = randindex();
    });
        run(A, B, C);


    }

} // end namespace