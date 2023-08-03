#!/bin/sh

#singularity build ../singularity-images/chplx-dev.sif Singularity.def
singularity run --bind $(pwd):/root/chplx -i ../singularity-images/chplx-dev.sif

