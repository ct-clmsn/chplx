#!/bin/bash

calc() { awk "BEGIN{print $*}"; }

doit_hpx() {
    echo "Benchmarking HPX"
    for i in {4..40}; do
        sum=0.000000
        for j in {1..5}; do
            local opt=$(./heat --hpx:threads=$i)
            sum=$(calc "$sum + $(echo $opt | awk -F',' '{print $7}')")
        done
        local _procs_=$(echo $opt | awk -F',' '{print $4}')
        echo "$_procs_,$(calc $sum / 5)"
    done
}

doit_chapel() {
    echo "Benchmarking Chapel"
    for i in {4..40}; do
        sum=0.000000
        for j in {1..5}; do
            local opt=$(CHPL_RT_NUM_THREADS_PER_LOCALE_QUIET=yes MAX_LOGICAL=$i MAX_PHYSICAL=$i CHPL_RT_NUM_THREADS_PER_LOCALE=$i ./heat_chapel)
            sum=$(calc "$sum + $(echo $opt | awk -F',' '{print $7}')")
        done
        local _procs_=$(echo $opt | awk -F',' '{print $4}')
        echo "$_procs_,$(calc $sum / 5)"
    done
}

doit_hpx
doit_chapel