#!/bin/bash
#SBATCH --job-name=classdemo
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=1  #(single process)
#SBATCH --cpus-per-task=4  #(32 class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:05:00
#SBATCH -o Part1a_4threads
#SBATCH -e err
export OMP_NUM_THREADS=4
./a.out 60 12 10

