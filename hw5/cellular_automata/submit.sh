#!/bin/bash
#SBATCH --job-name=cellular_mpi    # Job name

mpirun ./wolfram_ca.py 110 5000 1000 --periodic

echo "Done"
