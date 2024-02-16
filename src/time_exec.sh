#!/bin/bash
OUTFILE=./bench/v5.time

ARR_COUNT=100000
VAL_LIM=0
N_RUNS=500
WARMUP_RUNS=5
SEED=666

clear && make clean && make

echo >> $OUTFILE 
echo "#### ARR_COUNT="$ARR_COUNT", VAL_LIM="$VAL_LIM", N_RUNS="$N_RUNS", -O2" >> $OUTFILE 

RUN_ASM=1
echo -n "   ASM:" >> $OUTFILE 
./quicksort $ARR_COUNT $VAL_LIM $WARMUP_RUNS $SEED $RUN_ASM  # warmup
{ time ./quicksort $ARR_COUNT $VAL_LIM $N_RUNS $SEED $RUN_ASM; } 2>> $OUTFILE 

RUN_ASM=0
echo -n "   C:" >> $OUTFILE 
./quicksort $ARR_COUNT $VAL_LIM $WARMUP_RUNS $SEED $RUN_ASM  # warmup
{ time ./quicksort $ARR_COUNT $VAL_LIM $N_RUNS $SEED $RUN_ASM; } 2>> $OUTFILE 
