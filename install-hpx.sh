#!/bin/sh

mkdir -p /tmp/hpx
old_dir_="$(pwd)"
cd /tmp/hpx || exit 1
git clone --branch master --single-branch --depth 1 \
    https://github.com/STEllAR-GROUP/hpx.git
mkdir -p build
cd build || (cd "$old_dir_" && exit 1)
cmake \
    ../hpx \
    -GNinja \
    -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
    -DCMAKE_BUILD_TYPE=Release \
    -DHPX_WITH_CXX_STANDARD=20 \
    -DHPX_WITH_UNITY_BUILD=ON \
    -DHPX_WITH_MALLOC=system \
    -DHPX_WITH_EXAMPLES=OFF \
    -DHPX_WITH_TESTS=OFF
ninja install
cd "$old_dir_" || exit 1