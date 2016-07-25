#!/bin/bash

echo "serial 1000"

time ./matrix-serial 1000

echo "2"

echo "1000"
time mpirun -np 2 ./matrix 1000
echo "5000"
time mpirun -np 2 ./matrix 5000

echo "4"

echo "1000"
time mpirun -np 4 ./matrix 1000
echo "5000"
time mpirun -np 4 ./matrix 5000

echo "8"

echo "1000"
time mpirun -np 8 ./matrix 1000
echo "5000"
time mpirun -np 8 ./matrix 5000

echo "10"

echo "1000"
time mpirun -np 10 ./matrix 1000
echo "5000"
time mpirun -np 10 ./matrix 5000