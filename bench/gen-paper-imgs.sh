#!/bin/bash

tests="tree-2 ibk par-8-10 long-1000 long-10 par-2-10 par-8-100 tree-10"
#tests="long-1000"

for t in $tests
do
	nice ./bench/timejob.sh $t
#	./bench/plotthree.sh time/$t-*-avg.txt | gnuplot > imgs/$t.eps
	python ./bench/plot.py $t
done 
