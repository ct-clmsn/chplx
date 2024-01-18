#!/bin/bash

set -e

export PATH="/opt/bin:$PATH"
TEMP_HEAT_DIR="/tmp/chplx-heat-$$"
CURRENT_DIR="$(pwd)"
export CC=clang-15
export CXX=clang++-15
export APEX_DISABLE=1 # disable apex by default
VERBOSE=0
APEX_INCL=0
APEX_CMD="/opt/bin/apex_exec --apex:otf2 --apex:csv --apex:gtrace --apex:taskgraph --apex:postprocess --apex:source --apex:cpuinfo --apex:meminfo --apex:status"
APEX_STORE_DIR="$HOME"/apex.$$

build_chplx_no_flags() {
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DCHPL_HOME="$CURRENT_DIR"/frontend -DCMAKE_CXX_FLAGS="-undefined dynamic_lookup" -G Ninja 2>/dev/null 1>/dev/null #TODO: Generalize
    ninja >/dev/null
    cd "$CURRENT_DIR"
}

build_chplx_debug_flag() {
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DCHPL_HOME="$CURRENT_DIR"/frontend -DCMAKE_CXX_FLAGS="-undefined dynamic_lookup" -DCMAKE_BUILD_TYPE=Debug -G Ninja 2>/dev/null 1>/dev/null #TODO: Generalize
    ninja >/dev/null
    cd "$CURRENT_DIR"
}

build_chplx_reldebug_flag() {
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DCHPL_HOME="$CURRENT_DIR"/frontend -DCMAKE_CXX_FLAGS="-undefined dynamic_lookup" -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja 2>/dev/null 1>/dev/null #TODO: Generalize
    ninja >/dev/null
    cd "$CURRENT_DIR"
}

build_chplx_rel_flag() {
    mkdir -p build
    cd build
    rm -rf *
    chpl "$CURRENT_DIR"/publications/fall2023/stream.chpl -o "$CURRENT_DIR"/build/stream_chapel >/dev/null
    cmake .. -DCHPL_HOME="$CURRENT_DIR"/frontend -DCMAKE_CXX_FLAGS="-undefined dynamic_lookup -DCHPLX_NO_SOURCE_LOCATION" -DCMAKE_BUILD_TYPE=Release -G Ninja 2>/dev/null 1>/dev/null #TODO: Generalize
    ninja >/dev/null

    mkdir -p stream_chplx
    ./backend/chplx -f ../backend/test/stream_cpp.chpl -o stream_chplx
    cd stream_chplx
    mkdir -p build
    cd build
    cmake .. -G Ninja -DChplx_DIR="$CURRENT_DIR"/build/library/lib/cmake/Chplx -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-DCHPLX_NO_SOURCE_LOCATION" 2>/dev/null 1>/dev/null
    ninja >/dev/null
    cd "$CURRENT_DIR"
}

build_heat_release() {
    cd "$TEMP_HEAT_DIR"
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DChplx_DIR="$CURRENT_DIR"/build/library/lib/cmake/Chplx -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-DCHPLX_NO_SOURCE_LOCATION" -G Ninja 2>/dev/null 1>/dev/null #TODO: generalize
    ninja >/dev/null

    chpl "$CURRENT_DIR"/publications/fall2023/heat.chpl --fast -o "$CURRENT_DIR"/build/heat_chapel >/dev/null

    echo "Release builds (Chplx with Release build type and heat with release build type) amd chpl with --fast"
}

build_heat_relwithdebinfo() {
    cd "$TEMP_HEAT_DIR"
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DChplx_DIR="$CURRENT_DIR"/build/library/lib/cmake/Chplx -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja 2>/dev/null 1>/dev/null #TODO: generalize
    ninja >/dev/null

    chpl "$CURRENT_DIR"/publications/fall2023/heat.chpl --fast -o "$CURRENT_DIR"/build/heat_chapel >/dev/null

    echo "Chplx Release with Debug info and chpl with --fast"
}

build_heat_no_flags() {
    cd "$TEMP_HEAT_DIR"
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DChplx_DIR="$CURRENT_DIR"/build/library/lib/cmake/Chplx -G Ninja 2>/dev/null 1>/dev/null #TODO: generalize
    ninja >/dev/null

    chpl "$CURRENT_DIR"/publications/fall2023/heat.chpl -o "$CURRENT_DIR"/build/heat_chapel >/dev/null

    echo "No flags to both"
}

build_heat_no_flags_chpl_baseline() {
    cd "$TEMP_HEAT_DIR"
    mkdir -p build
    cd build
    rm -rf *
    cmake .. -DChplx_DIR="$CURRENT_DIR"/build/library/lib/cmake/Chplx -DCMAKE_BUILD_TYPE=Debug -G Ninja 2>/dev/null 1>/dev/null #TODO: generalize
    ninja >/dev/null

    chpl "$CURRENT_DIR"/publications/fall2023/heat.chpl --baseline -o "$CURRENT_DIR"/build/heat_chapel >/dev/null

    echo "HPX Debug and chapel with --baseline"
}

calc() { awk "BEGIN{print $*}"; }

doit_hpx() {
    echo "Benchmarking ChplX"
    local _dir_meta_name_="$1"
    local _prefix_=""
    if [ $APEX_INCL -eq 1 ]; then
        _prefix_="${APEX_CMD}"
    fi
    for i in {4..64}; do
        sum=0.000000
        mkdir -p "$APEX_STORE_DIR/$_dir_meta_name_/proc$i"
        for j in {1..5}; do
            local opt=$($_prefix_ $CURRENT_DIR/build/stream_chplx/build/stream_cpp --hpx:threads=$i)
            #echo $opt
            #sum=$(calc "$sum + $(echo $opt | awk -F',' '{print $7}')")
            #echo "$opt" >"$APEX_STORE_DIR/$_dir_meta_name_/proc$i"/runlog
        done
        #local _procs_=$(echo $opt | awk -F',' '{print $4}')
        #echo "$_procs_,$(calc $sum / 5)"
        echo $opt
        if [ $APEX_INCL -eq 1 ]; then
            cp -r $TEMP_HEAT_DIR/build/OTF2_archive "$APEX_STORE_DIR/$_dir_meta_name_/proc$i"
        fi
    done
}

doit_chapel() {
    echo "Benchmarking Chapel"
    for i in {4..64}; do
        sum=0.000000
        for j in {1..5}; do
            local opt=$(CHPL_RT_NUM_THREADS_PER_LOCALE_QUIET=yes MAX_LOGICAL=$i MAX_PHYSICAL=$i CHPL_RT_NUM_THREADS_PER_LOCALE=$i "$CURRENT_DIR"/build/stream_chapel)
            #echo $opt
            #sum=$(calc "$sum + $(echo $opt | awk -F',' '{print $7}')")
        done
        #local _procs_=$(echo $opt | awk -F',' '{print $4}')
        #echo "$_procs_,$(calc $sum / 5)"
        echo $opt
    done
}

checks() {
    if [ -d ./.git ]; then
        echo "This $(pwd) is assumed to be the parent directory of the chplx project"
    else
        echo ".git not found at $(pwd)"
        echo "exiting!"
        exit 1
    fi

    if git status | grep -q "paper_patches"; then
        echo "paper_patches is the correct branch"
    else
        echo "Checkout the paper_patches branch and try again!"
        exit 1
    fi
}

main() {
    # make temporary directories
    mkdir -p "$TEMP_HEAT_DIR"

    
    cd "$CURRENT_DIR"

    build_chplx_rel_flag
    doit_hpx "release"
    doit_chapel
    cd "$CURRENT_DIR"

    
    echo "Benchmarking done"
}

usage() {
    echo "Usage: $0 -v (verbose)" 1>&2
    echo "Usage: $0 -v (verbose) -a (apex)" 1>&2
    echo "Usage: $0 -v (verbose) -A (<path of directory where apex data will be copied>)" 1>&2
    exit 1
}

if ! command -v chpl >/dev/null; then
    echo "chpl needs to be installed or not in PATH"
    exit 1
fi

while getopts "vaA:" o; do
    case "${o}" in
    v)
        VERBOSE=1
        ;;
    A)
        APEX_INCL=1
        if [ "${OPTARG}" != "" ]; then
            if [ -d "${OPTARG}" ]; then
                APEX_STORE_DIR="${OPTARG}"
            else
                echo "${OPTARG} directory is not present"
                exit 1
            fi
        fi
        export APEX_DISABLE=0
        echo "Apex data will be stored in $APEX_STORE_DIR"
        ;;
    a)
        export APEX_DISABLE=0
        APEX_INCL=1
        echo "Apex data will be stored in $APEX_STORE_DIR"
        ;;
    *)
        echo "Unknown argument: ${OPTARG}"
        exit 1
        ;;
    esac
done
shift $((OPTIND - 1))

if [ $VERBOSE == 1 ]; then
    echo "Verbose mode"
    set -x
fi

main
