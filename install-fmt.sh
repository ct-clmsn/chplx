#!/bin/sh

mkdir - p /tmp/fmt
cd /tmp/fmt || exit 1
git clone --branch 9.1.0 --single-branch --depth 1 \
    https://github.com/fmtlib/fmt.git
mkdir -p build
cd build || exit 1
cmake ../fmt -GNinja \
    -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DFMT_TEST=OFF
ninja install
