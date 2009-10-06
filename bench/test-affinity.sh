#!/bin/bash

APP=./build-nonshared/cd3
MODEL=$1
echo $MODEL
TC=8
[ ! -z $2 ] && TC=$2
TYPES="compact disabled none scatter"

if [ ! -f data/models/paper/$MODEL.xml ]
then
	echo "nix da model da"
	exit
fi

for type in $TYPES
do
	echo "doing for type" $type
	export KMP_AFFINITY="$type"
	export OMP_NUM_THREADS=$TC
	echo "thread count is $TC"
	for i in `seq 3`
	do
		nice -n -20 $APP -v 1 data/models/paper/$MODEL.xml | awk '/simulation took/ {print $9}'
	done
done
