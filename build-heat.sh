#!/bin/sh

cd backend/test/heat
mkdir -p build
cd build && rm -rf *
cmake .. -G Ninja -DChplx_DIR=/root/chplx/build/library/lib/cmake/Chplx/
ninja

