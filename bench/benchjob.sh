#!/bin/bash

APP=./cd3
MODEL=nono
THREADS=10
RUNS=5
TIME_OUT="time.txt"
AVG_FILE="time-avg.txt"

[ ! -z $1 ] && MODEL=$1
[ ! -z $2 ] && THREADS=$2
[ ! -z $3 ] && RUNS=$3

echo "starting" 

if [ ! -d imgs ]
then
	echo "creating imgs folder"
	mkdir imgs
fi

if [ -x $TIME_OUT ]
then
	echo "deleting bench.txt"
	/bin/rm $TIME_OUT
fi 

if [ -x $APP ]
then
	echo "running benchmarks"
	#./runbench.sh $APP
	
	for i in $(seq $THREADS); do
		for j in $(seq $RUNS); do
			export OMP_NUM_THREADS=$i
			#/usr/bin/time -f "$i\t%e\t%S\t%U" -a -o $TIME_OUT $APP $MODEL > /dev/null
			echo -n "$i 	" >> $TIME_OUT
			LD_PRELOAD=/usr/lib/libtcmalloc.so $APP models/paper/$MODEL 2>> $TIME_OUT
			#$APP models/paper/$MODEL 2>> $TIME_OUT
		done
	done
fi

echo "averaging results"
./bench/bench.py $TIME_OUT > $AVG_FILE
echo "plotting results"
TMP_FILE="imgs/benc$MODEL-$$.eps"
#TMP_FILE=$(tempfile -d imgs -s .ps -p $MODEL)
gnuplot bench/plotwall.gp > $TMP_FILE
xdg-open $TMP_FILE
echo "cleaning up"
/bin/rm $TIME_OUT
/bin/rm $AVG_FILE
