#!/bin/bash
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=8
#SBATCH --mem-per-cpu=100mb
#SBATCH -t 00:05:00
#SBATCH -o outfile_60_12_10
#SBATCH -e errfile
export OMP_NUM_THREADS=8
srun --mpi=pmix_v3 ./mpi_omp_matmult 60 12 10