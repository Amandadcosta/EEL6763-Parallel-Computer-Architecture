#!/bin/bash
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=100mb
#SBATCH -t 00:05:00
#SBATCH -o Table1_200_Row1
#SBATCH -e errfile
export OMP_NUM_THREADS=8
srun --mpi=pmix_v3 ./mpi_omp_matmult 200 200 200