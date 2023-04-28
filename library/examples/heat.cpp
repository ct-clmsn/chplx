//  Copyright (c) 2023 Steve R. Brandt
//  Copyright (c) 2023 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Notes on running:
// (1) This program does not support positional args.
// (2) Use --nx=YYY to set the number of cells to YYY.
// (3) use --nt=YYY to set the number of time steps to YYY.
//
// ./heat --nx=10000000 --hpx:threads=6

#include <chplx.hpp>

#include <hpx/chrono.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/program_options.hpp>

#include <cstdlib>

using namespace chplx;

// use Time;
//
// config const ghosts: int = 1;
// config const k: real = 0.4;
// config const dt: real = 1.0;
// config const dx: real = 1.0;
//
// config const nx: int = 1_000_000;
// config const nt: int = 100;

int ghosts = 1;
double k = 0.4;
double dt = 1.0;
double dx = 1.0;

int nx = 1'000'000;
int nt = 100;

hpx::program_options::options_description get_config_variables() {
  hpx::program_options::options_description options;

  // clang-format off
  options.add_options()
    ("ghosts",
          hpx::program_options::value<int>(&ghosts),
          "config const ghosts: int = 1;")
    ("k",
          hpx::program_options::value<double>(&k),
          "config const k: real = 0.4;")
    ("dt",
          hpx::program_options::value<double>(&dt),
          "config const dt: real = 1.0;")
    ("dx",
          hpx::program_options::value<double>(&dx),
          "config const dx: real = 1.0;")
    ("nx",
          hpx::program_options::value<int>(&nx),
          "config const nx: int = 1_000_000;")
    ("nt",
          hpx::program_options::value<int>(&nt),
          "config const nt: int = 100;")
  ;
  // clang-format on

  return options;
}

// proc update(d : []real, d2 : []real) {
//   forall i in 1..NX-1 do {
//     d2[i] = d[i] + dt*k/(dx*dx)*(d[i+1] + d[i-1] - 2*d[i]);
//   }
//   d2[0] = d2[NX-1];
//   d2[NX] = d2[1];
// }

void update(auto &&d, auto &&d2) {

  forall(
      Range(1, d.size() - 1),
      [](auto i, auto &&d, auto &&d2) {
        d2[i] = d[i] + dt * k / (dx * dx) * (d[i + 1] + d[i - 1] - 2 * d[i]);
      },
      d, d2);
  d2[0] = d2[d.size() - 1];
  d2[d.size()] = d2[1];
}

// proc main() {
//   data2 = 0;
//   forall i in 0..NX do {
//     data[i] = 1 + (i-1 + nx) % nx;
//   }
//   var t: stopwatch;
//   t.start();
//   for t in 1..nt do {
//     update(data, data2);
//     data <=> data2;
//   }
//   t.stop();
//   if data.size < 20 {
//     writeln(data);
//   }
//   writeln("chapel,",nx,",",nt,",",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string,",",dt,",",dx,",",t.elapsed(),",0");
// }

// var data: [0..NX] real;
// var data2: [0..NX] real;

void main_proc() {

  std::int64_t const NX = nx + 1;

  Array data(Domain(Range(0, NX)), 0.0);
  Array data2(Domain(Range(0, NX)), 0.0);

  forLoop(Range(0, NX), [&](auto i) { data[i] = 1 + (i - 1 + nx) % nx; });

  hpx::chrono::high_resolution_timer const t;

  forLoop(Range(1, nt), [&](auto) {
    update(data, data2);
    using std::swap;
    swap(data, data2);
  });

  auto elapsed = t.elapsed();

  if (data.size() < 20) {
    writeln(data);
  }

  writeln("chapel,", nx, ",", nt, ",", getenv("CHPL_RT_NUM_THREADS_PER_LOCALE"),
          ",", dt, ",", dx, ",", elapsed, ",0");
}

int hpx_main(int argc, char *argv[]) {

  main_proc();
  return hpx::finalize();
}

int main(int argc, char *argv[]) {

  hpx::program_options::options_description desc_commandline;
  desc_commandline.add(get_config_variables());

  hpx::init_params init_args;
  init_args.desc_cmdline = desc_commandline;

  return hpx::init(argc, argv, init_args);
}
