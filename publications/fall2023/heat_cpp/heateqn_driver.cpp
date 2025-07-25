// This program file was generated by the chplx compiler.
// The original Chapel program file can be found here: heateqn.chpl
//
#include <hpx/hpx_init.hpp>
#include <hpx/program_options.hpp>

#include "heateqn_driver.hpp"

using namespace heateqn;

int hpx_main(int argc, char * argv[]) {
    heateqn::__this->__main();
    return hpx::finalize();
}

int main(int argc, char * argv[]) {
    chplx::registerModule<heateqn::__thisModule>();
    hpx::program_options::options_description options;
    options.add_options()
        ("ghosts",
            hpx::program_options::value(&heateqn::ghosts), "config const ghosts : std::int64_t")
        ("k",
            hpx::program_options::value(&heateqn::k), "config const k : double")
        ("dt",
            hpx::program_options::value(&heateqn::dt), "config const dt : double")
        ("dx",
            hpx::program_options::value(&heateqn::dx), "config const dx : double")
        ("nx",
            hpx::program_options::value(&heateqn::nx), "config const nx : std::int64_t")
        ("nt",
            hpx::program_options::value(&heateqn::nt), "config const nt : std::int64_t");
    hpx::init_params init_args;
    init_args.desc_cmdline = options;
    return hpx::init(argc, argv, init_args);
}
