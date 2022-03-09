#!/bin/bash
#SBATCH --job-name=classdemo
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=1  #(single process)
#SBATCH --cpus-per-task=8  #(32 class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:05:00
#SBATCH -o Part1_Fig2_8threads_800
#SBATCH -e err
export OMP_NUM_THREADS=8
./a.out 800 800 800

