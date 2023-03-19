//  Copyright (c) 2023 Steve R. Brandt
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <chplx.hpp>

#include <hpx/chrono.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/program_options.hpp>

#include <cstdlib>

using namespace chplx;

// config const ghosts: int = 1;
// config const k: real = 0.4;
// config const dt: real = 1.0;
// config const dx: real = 1.0;
//
// config const nx: int = 1_000_000;
// config const nt: int = 100;
// config const nthreads: int = 1;
int ghosts = 1;
double k = 0.4;
double dt = 1.0;
double dx = 1.0;

int nx = 1'000'000;
int nt = 100;

int nthreads = 1;

// Every chapel module is represented by a singleton instance of a corresponding
// type and can be accessed through a global pointer that is initialized during
// startup.
struct moduleMain *__moduleMain = nullptr;

struct moduleMain {

  // Every module has to expose a static function __config_variables that should
  // return the command line options for all config variables that are exposed
  // by this module.
  static hpx::program_options::options_description __config_variables() {
    hpx::program_options::options_description options;

    // clang-format off
    options.add_options()
      ("ghosts",
            hpx::program_options::value(&ghosts),
            "config const ghosts: int = 1;")
      ("k",
            hpx::program_options::value(&k),
            "config const k: real = 0.4;")
      ("dt",
            hpx::program_options::value(&dt),
            "config const dt: real = 1.0;")
      ("dx",
            hpx::program_options::value(&dx),
            "config const dx: real = 1.0;")
      ("nx",
            hpx::program_options::value(&nx),
            "config const nx: int = 1_000_000;")
      ("nt",
            hpx::program_options::value(&nt),
            "config const nt: int = 100;")
      ("threads",
            hpx::program_options::value(&nthreads),
            "config const nthreads: int = 1;")
    ;
    // clang-format on

    return options;
  }

  // const alp : real = k*dt/(dx*dx);
  double const alp = k * dt / (dx * dx);

  // const tx : int = (2*ghosts+nx)/nthreads;
  int const tx = (2 * ghosts + nx) / nthreads;

  // const qsize : int = 12;
  int const qsize = 12;

  // var qarray : [0..2*qsize*nthreads] real;
  // var qhead : [0..2*nthreads] int;
  // var qtail : [0..2*nthreads] int;
  Array<double, Domain<1, int>> qarray =
      Array(Domain(Range(0, 2 * qsize * nthreads)), 0.0);
  Array<int, Domain<1, int>> qhead = Array(Domain(Range(0, 2 * nthreads)), 0);
  Array<int, Domain<1, int>> qtail = Array(Domain(Range(0, 2 * nthreads)), 0);

  // var data : [0..nx-1] real;
  Array<double, Domain<1, int>> data = Array(Domain(Range(0, nx - 1)), 0.0);

  // const leftq = 1;
  // const rightq = 0;
  int const leftq = 1;
  int const rightq = 0;

  // proc push_queue(left_right : int, threadno : int, val : real) {
  //     const qno : int = left_right + 2 * threadno;
  //     const t : int = qtail[qno];
  //     const idx : int = qno * qsize + t % qsize + 1;
  //     qarray[idx] = val;
  //     qtail[qno] += 1;
  // }
  void push_queue(int left_right, int threadno, double val) {
    int const qno = left_right + 2 * threadno;
    int const t = qtail[qno];
    int const idx = qno * qsize + t % qsize + 1;
    qarray[idx] = val;
    qtail[qno] += 1;
  }

  // proc pop_queue(left_right : int, threadno : int ) {
  //     const qno = left_right + 2 * threadno;
  //     const h = qhead[qno];
  //     const idx = qno * qsize + h % qsize + 1;
  //     var t = qtail[qno];
  //     while h == t {
  //         sleep(0);
  //         t = qtail[qno];
  //     }
  //     const val = qarray[idx];
  //     qhead[qno] += 1;
  //     return val;
  // }
  double pop_queue(int left_right, int threadno) {
    auto const qno = left_right + 2 * threadno;
    auto const h = qhead[qno];
    auto const idx = qno * qsize + h % qsize + 1;
    auto t = qtail[qno];
    while (h == t) {
      hpx::this_thread::yield();
      t = qtail[qno];
    }
    auto const val = qarray[idx];
    qhead[qno] += 1;
    return val;
  }

  // proc work(num:int) {
  //    //print("Start work: ",num,"\n")
  //    var lo : int = tx*num;
  //    var hi : int = tx*(num+1);
  //    if hi > nx {
  //        hi = nx;
  //    }
  //    lo -= ghosts;
  //    hi += ghosts;
  //    const sz = hi - lo;
  //    var data1 : [0..sz-1] real;
  //    var data2 : [0..sz-1] real;
  //    const off = 1;
  //    const ip1 = (num + 1) % nthreads;
  //    const im1 = (num + nthreads - 1) % nthreads;
  //
  //    for i in 0..sz-1 {
  //        data1[i] = i + lo + off;
  //    }
  //
  //    // send
  //    push_queue(rightq, im1, data1[1]);
  //    push_queue(leftq, ip1, data1[sz-2]);
  //    //print("Pre loop: ",num,"\n")
  //    for nt in 1..nt {
  //        //print("nt: ",nt," ",data1,"\n")
  //        data1[0] = pop_queue(leftq, num);
  //        data1[sz-1] = pop_queue(rightq, num);
  //
  //        //print("Update: nt: ",nt," num: ",num,"\n")
  //        for i in 1..sz-2 {
  //            //print("Data i: ",i,"\n")
  //            data2[i] = data1[i] + alp*(data1[i+1] + data1[i-1] -
  //            2*data1[i]);
  //        }
  //        //print("Post Update: nt: ",nt," num: ",num,"\n")
  //        push_queue(rightq, im1, data1[1]);
  //        push_queue(leftq, ip1, data1[sz-2]);
  //        // swap
  //        data1 <=> data2;
  //    }
  //    //print("End Evo\n")
  //    data1[0] = pop_queue(leftq, num);
  //    data1[sz-1] = pop_queue(rightq, num);
  //    for i in 0..sz-1 {
  //      var j = i + lo;
  //      if j >= 0 && j < nx {
  //        data[j] = data1[i];
  //      }
  //    }
  //}
  void work(int num) {

    int lo = tx * num;
    int hi = tx * (num + 1);
    if (hi > nx) {
      hi = nx;
    }
    lo -= ghosts;
    hi += ghosts;

    auto const sz = hi - lo;
    Array data1(Domain(Range(0, sz - 1)), 0.0);
    Array data2(Domain(Range(0, sz - 1)), 0.0);

    auto const off = 1;
    auto const ip1 = (num + 1) % nthreads;
    auto const im1 = (num + nthreads - 1) % nthreads;

    forLoop(Range(0, sz - 1), [&](auto i) { data1[i] = i + lo + off; });

    // send
    push_queue(rightq, im1, data1[1]);
    push_queue(leftq, ip1, data1[sz - 2]);

    forLoop(Range(1, nt), [&](auto nt) {
      data1[0] = pop_queue(leftq, num);
      data1[sz - 1] = pop_queue(rightq, num);

      forLoop(Range(1, sz - 2), [&](auto i) {
        data2[i] =
            data1[i] + alp * (data1[i + 1] + data1[i - 1] - 2 * data1[i]);
      });

      push_queue(rightq, im1, data1[1]);
      push_queue(leftq, ip1, data1[sz - 2]);

      using std::swap;
      swap(data1, data2);
    });

    data1[0] = pop_queue(leftq, num);
    data1[sz - 1] = pop_queue(rightq, num);
    forLoop(Range(0, sz - 1), [&](auto i) {
      auto j = i + lo;
      if (j >= 0 && j < nx) {
        data[j] = data1[i];
      }
    });
  }

  // proc main() {
  //  var t: stopwatch;
  //  t.start();
  //  forall tno in 0..nthreads - 1 do {
  //    work(tno);
  //  }
  //  t.stop();
  //  if(nx <= 20) {
  //    writeln(data);
  //  }
  //  writeln("chapel,",nx,",",nt,",",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string,",",dt,",",dx,",",t.elapsed(),",0");
  //}
  void __main() {

    hpx::chrono::high_resolution_timer const t;

    forall(Range(0, nthreads - 1), [&](auto tno) { work(tno); });

    auto elapsed = t.elapsed();

    if (data.size() < 20) {
      writeln(data);
    }

    writeln("chapel,", nx, ",", nt, ",",
            getenv("CHPL_RT_NUM_THREADS_PER_LOCALE"), ",", dt, ",", dx, ",",
            elapsed, ",0");
  }

  // __construct/__destruct are static helper functions that are being used to
  // ensure the module's variables are initialized once HPX is up and running
  // and are destroyed before HPX exits (see chplx::registerModule below)
  static void __construct() { __moduleMain = new moduleMain(); }
  static void __destruct() noexcept {
    auto *m = __moduleMain;
    __moduleMain = nullptr;
    delete m;
  }
};

int hpx_main(int argc, char *argv[]) {

  __moduleMain->__main();

  return hpx::finalize();
}

int main(int argc, char *argv[]) {

  // register main module
  registerModule<moduleMain>();

  hpx::program_options::options_description desc_commandline;
  desc_commandline.add(moduleMain::__config_variables());

  hpx::init_params init_args;
  init_args.desc_cmdline = desc_commandline;

  return hpx::init(argc, argv, init_args);
}
