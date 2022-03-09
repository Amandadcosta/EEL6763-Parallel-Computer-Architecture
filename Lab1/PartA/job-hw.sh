#!/bin/bash
#SBATCH --job-name=partA_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=a.dcosta@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o output1
#SBATCH -e err1
srun --mpi=pmix_v2 ./MatmultPart1
