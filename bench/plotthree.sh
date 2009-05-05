#!/usr/bin/env bash

if [ -z $1 ]; then
    echo "usage: three models"
    exit -1
fi

if [ -z $2 ]; then
    echo "usage: three models"
    exit -1
fi

if [ -z $3 ]; then
    echo "usage: three models"
    exit -1
fi

MODEL_PAR=$2
MODEL_PIPE=$3
MODEL_ORDERED=$1

mpfile="set terminal png enhanced large\n\
set output\n\
set yrange [0:]\n\
set ylabel \"runtime\"\n\
set xlabel \"number of threads\"\n\
plot 	'%s' using 1:2 with lines title \"parallel\", '%s' using 1:2 with lines title \"piped\", '%s' using 1:2 with lines title \"ordered\""

AVG_FILE="time/$MODEL-$$-time-avg.txt"

printf "$mpfile" $MODEL_PAR $MODEL_PIPE $MODEL_ORDERED 

