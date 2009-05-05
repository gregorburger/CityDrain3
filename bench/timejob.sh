#!/bin/bash

APP=./cd3
THREADS=10
RUNS=4

if [ -z $1 ]; then
    echo "usage: model [nThreads=$THREADS] [nRuns=$RUNS] [cd3bin=$APP]"
    exit -1
fi
MODEL=$1
[ ! -z $2 ] && THREADS=$2
[ ! -z $3 ] && RUNS=$3
[ ! -z $4 ] && APP=$4

if [ ! -x $APP ]
then
    echo "cd3 binary $APP does no exist"
    exit -2
fi

echo "starting" 

if [ ! -d imgs ]
then
	echo "creating imgs folder"
	mkdir imgs
fi

if [ ! -d time ]
then
    echo "creating time folder"
    mkdir time
fi

TIME_OUT="time/$MODEL-$$-time.txt"
AVG_FILE="time/$MODEL-$$-time-avg.txt"

echo "running benchmarks"
#./runbench.sh $APP

for i in $(seq $THREADS); do
	for j in $(seq $RUNS); do
		export OMP_NUM_THREADS=$i
		echo -n "$i 	" >> $TIME_OUT
		#LD_PRELOAD=/usr/lib/libtcmalloc.so 
		$APP models/paper/$MODEL 2>> $TIME_OUT
	done
done

echo "averaging results"
./bench/bench.py $TIME_OUT > $AVG_FILE
echo "plotting results"
cp $AVG_FILE time-avg.txt
TMP_FILE="imgs/bench-$MODEL-$$.png"
gnuplot bench/plotwall.gp > $TMP_FILE
rm time-avg.txt
