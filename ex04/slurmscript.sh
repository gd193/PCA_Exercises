#!/usr/bin/env bash
#SBATCH --partition=bench
#SBATCH --nodes 1

for matrixSize in 10 100 500 1000 5000 10000
do
	for thread in 1 2 4 8 16 32
	do
                echo ./ex04 $matrixSize $thread
		./ex04 $matrixSize $thread
	done
done
