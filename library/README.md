chplx

Quickstart

This directory contains the support library needed to compile the generated code.

- library

  directory contains the definition of types needed to support the generated code;

  - compile the frontend and backend as per the instructions in `../README.md`
  - `cd library`
  - `mkdir build`
  - `cd build`
  - `cmake -DClang_DIR=$(PATH_TO_LLVM)/lib/cmake/clang -DLLVM_DIR=$(PATH_TO_LLVM)/lib/cmake/llvm -Dfmt_DIR=$(PATH_TO_FMT)/lib/cmake/fmt ..`

- license

  the support is under the Boost Software License Version 1.0

  https://www.boost.org/LICENSE_1_0.txt

- date

  Feb, 2023
