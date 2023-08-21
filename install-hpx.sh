#!/bin/sh

mkdir -p /tmp/hpx
old_dir_="$(pwd)"

_cmake_version_="$(git -c 'versionsort.suffix=-' \
  ls-remote --tags --sort='v:refname' \
  https://github.com/Kitware/CMake.git |\
  grep -v beta | grep -v rc |\
  tail --lines=1 | cut --delimiter='/' --fields=3 |\
  sed 's/\^{}//g')"
_cmake_version_nv_=$(echo $_cmake_version_ | sed 's/^v//g')
cd /tmp
wget https://github.com/Kitware/CMake/releases/download/${_cmake_version_}/cmake-${_cmake_version_nv_}-linux-x86_64.tar.gz
tar -xvzf ./cmake-$_cmake_version_nv_-linux-x86_64.tar.gz
cd cmake-$_cmake_version_nv_-linux-x86_64
cp -r bin/* /usr/local/bin
cp -r doc/* /usr/local/doc
cp -r share/* /usr/local/share
cp -r man/* /usr/local/share/man

cd /tmp

rm -rf /tmp/otf2-3.0.3.tar.gz
rm -rf /tmp/v2.6.2.tar.gz

cd /tmp/
rm -rf apex


wget https://perftools.pages.jsc.fz-juelich.de/cicd/otf2/tags/otf2-3.0.3/otf2-3.0.3.tar.gz
tar -xvzf otf2-3.0.3.tar.gz
cd otf2-3.0.3
./configure --prefix=/usr
make -j
make install

cd /tmp
wget https://github.com/UO-OACISS/apex/archive/refs/tags/v2.6.2.tar.gz
tar -xvzf v2.6.2.tar.gz
cd apex-2.6.2
mkdir build
cd build
cmake .. -DAPEX_WITH_OFT2=ON -G Ninja
ninja

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
    -DHPX_WITH_TESTS=OFF \
    -DHPX_WITH_APEX=ON \
    -DAPEX_ROOT=/tmp/apex-2.6.2 \
    -DAPEX_WITH_OTF2=ON \
    -DHPX_WITH_APEX_NO_UPDATE=ON

ninja install
cd "$old_dir_" || exit 1