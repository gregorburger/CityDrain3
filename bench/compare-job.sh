#!/bin/bash

APP=cd3
THREADS=10
RUNS=4

TYPES="shared nonshared"

if [ -z $1 ]; then
    echo "usage: model [nThreads=$THREADS] [nRuns=$RUNS] [cd3bin=$APP]"
    exit -1
fi
MODEL=$1
[ ! -z $2 ] && THREADS=$2
[ ! -z $3 ] && RUNS=$3
[ ! -z $4 ] && APP=$4

for TYPE in $TYPES; do
	if [ ! -x build-$TYPE/$APP ]
	then
		echo "cd3 binary build-$TYPE/$APP does no exist"
		exit -2
	fi
done

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

PWD=`pwd`

for TYPE in $TYPES; do

	TIME_OUT="time/$MODEL-$TYPE-$$-time.txt"
	AVG_FILE="time/$MODEL-$TYPE-$$-time-avg.txt"
	TYPE_APP=./build-$TYPE/$APP
	echo $TYPE_APP
	
	for i in $(seq $THREADS); do
		for j in $(seq $RUNS); do
			export OMP_NUM_THREADS=$i
			echo -n "$i	" >> $TIME_OUT
#			export LD_LIBRARY_PATH=./build-$TYPE
			nice -n -20 $TYPE_APP $PWD/data/models/single/$MODEL.xml | awk '/simulation took/ {print $9}' >> $TIME_OUT
		done
	done

	./bench/bench.py $TIME_OUT > $AVG_FILE
done
python ./bench/plot-compare.py $MODEL $$
python ./bench/plot-compare-speedup.py $MODEL $$
