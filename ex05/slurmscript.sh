#!/usr/bin/env bash
#SBATCH --partition=bench
#SBATCH --nodes 1

for messagesize in {0..20}
do
    echo $messagesize 100
		/usr/lib64/openmpi/bin/mpirun -n 2 -mca btl tcp,self ./PingPong $messagesize 100
		/usr/lib64/openmpi/bin/mpirun -n 2 -mca btl tcp,self ./PingExchange $messagesize 100
done
