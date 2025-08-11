#!/bin/bash
#SBATCH --time=24:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=64
#SBATCH --exclusive
#SBATCH --partition=cuda-A100-intel
#SBATCH -o slurm-%j.out-%N # optional, name of the stdout, using the job number (%j) and the hostname of the node (%N)
#SBATCH -e slurm-%j.err-%N # optional, name of the stderr, using job and hostname values
 

singularity run --bind $(pwd):/root/chplx -i ../singularity-images/noapexHPX.sif /bin/bash -c "cd /root/chplx && bash bench.bash -v"

