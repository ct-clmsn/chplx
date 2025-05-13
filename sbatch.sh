#!/bin/bash
#SBATCH --job-name=chplx_benchmarks        # Job name
#SBATCH --output=R-%x.%j.out          # Standard output file
#SBATCH --error=R-%x.%j.err            # Standard error file
#SBATCH --partition=cuda-A100    # Partition or queue name
#SBATCH --nodes=1                     # Number of nodes
#SBATCH --mem=0                     # Number of nodes
#SBATCH --exclusive                     # Number of nodes
#SBATCH --time=4:00:00                # Maximum runtime (D-HH:MM:SS)

module load gcc/14.1.0
python3 benchmark.py 2>&1

