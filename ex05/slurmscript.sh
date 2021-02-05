#!/usr/bin/env bash
#SBATCH --partition=bench
#SBATCH --nodes 1

for messagesize in {0..20}
do
    echo ./ex04 $messagesize 100
		./ex04 $messagesize 100
	done
done
