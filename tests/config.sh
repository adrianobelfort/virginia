#!/bin/bash

gcc test/testgenerator.c -o test/generator
cd stats/
gcc metrics.c -o metrics-analyzer
cd ..
gcc common/smooth.c sequential/solution.c -o sequential/smooth -Isequential -fopenmp
gcc common/smooth.c parallel/solution.c -o parallel/smoothparallel -Iparallel -fopenmp
