c[hp]l
 [hp]x

Quickstart

This directory contains the backend.

- backend

  directory contains a compiler driver that uses the front end; compilation directions...

  - compile the frontend as per the instructions in `../README.md`
  - `cd backend`
  - `mkdir build`
  - `cd build`
  - `cmake -DClang_DIR=$(PATH_TO_LLVM)/lib/cmake/clang -DLLVM_DIR=$(PATH_TO_LLVM)/lib/cmake/llvm -Dfmt_DIR=$(PATH_TO_FMT)/lib/cmake/fmt ..`

  Backend usage example

  `./chplx -f ../../test/release/examples/primers/arrays.chpl`

  Note

  this tool does not import the Chapel standard library, the standard library code can be found in this path 

  `chapel/modules`

  from the chapel github repository; the standard library does
  include architecture/os specific chapel code

- license

  the backend is under the Boost Software License Version 1.0

  https://www.boost.org/LICENSE_1_0.txt

- date

  Feb, 2023
