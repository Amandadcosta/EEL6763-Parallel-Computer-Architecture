#!/bin/bash
#SBATCH --job-name=MonteCarloIntegration_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=a.dcosta@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=32
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o SendRecvR32N100000
#SBATCH -e part2err
srun --mpi=pmix_v2 ./Monte_Carlo_Integration -10 10 100000

