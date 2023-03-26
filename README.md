chplx

Quickstart

This project is split into 3 directories and 3 projects, frontend, library, & backend. 
All parts can be built using a unified build system.

Compilation directions...
```
  > mkdir build
  > cd build
  > cmake -DClang_DIR=$(PATH_TO_LLVM)/lib/cmake/clang \
          -DLLVM_DIR=$(PATH_TO_LLVM)/lib/cmake/llvm \
          -DCHPL_HOME=$(PATH_TO_CHAPEL) \
          -DHPX_DIR=$(PATH_TO_HPX)/lib/cmake/HPX \
          -Dfmt_DIR=$(PATH_TO_FMT)/lib/cmake/fmt \
          ..
  > cmake --target all
```
- frontend

  directory contains library code from the Chapel project, it is suggested that the chapel
  project be checked out from it's git repository `https://github.com/chapel-lang/chapel.git`

  - license

  The frontend is taken from the Chapel source directory. Small modifications have been made to the build scripts and
  some files have been moved around to get the front end sufficiently decoupled from the Chapel source tree. Chapel
  is licensed under the Apache License 2.0.

- library

  directory contains a C++ library used by the backend code generator.

  - license

  The backend is under the Boost Software License Version 1.0

  https://www.boost.org/LICENSE_1_0.txt

- backend

  directory contains a compiler driver that uses the front end; compilation directions...

  Backend usage example

  `./chplx -f ../../test/release/examples/primers/arrays.chpl`

  Note

  this tool does not import the Chapel standard library, the standard library code can be found in this path 

  `chapel/modules`

  from the chapel github repository; the standard library does
  include architecture/os specific chapel code

  - license

  The backend is under the Boost Software License Version 1.0

  https://www.boost.org/LICENSE_1_0.txt

- files

  - CONTRIBUTORS, list of individual contributors
  - THANKS, organizations and companies that made this effort possible

- date

  Feb, 2023
