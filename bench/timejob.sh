#!/bin/bash

APP=./build/cd3
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


for sim in par pipe ordered
do
	TIME_OUT="time/$MODEL-$sim-$$-time.txt"
	AVG_FILE="time/$MODEL-$sim-$$-time-avg.txt"
	
	echo $MODEL-$sim.xml
	
	for i in $(seq $THREADS); do
		for j in $(seq $RUNS); do
			export OMP_NUM_THREADS=$i
			echo -n "$i 	" >> $TIME_OUT
			nice -n -20 $APP -v 1 data/models/paper/$MODEL-$sim.xml | awk '/simulation took/ {print $9}' >> $TIME_OUT
		done
	done

	./bench/mins.py $TIME_OUT > $AVG_FILE
done
python ./bench/plot.py $MODEL $$
python ./bench/plot-speedup.py $MODEL $$
