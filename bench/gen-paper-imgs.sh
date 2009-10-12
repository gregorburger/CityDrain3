#!/bin/bash

#tests="tree-2 ibk par-8-10 long-1000 long-10 par-2-10 par-8-100 tree-10"
tests="ibk long-10 long-100 par-2-10 par-2-100 par-4-10 par-4-100 par-8-10 par-8-100 tree-2 tree-4 tree-7 tree-10 long-1000"
#tests="long-1000"
#tests="tree-2"

for t in $tests
do
	./bench/timejob.sh $t
#	./bench/plotthree.sh time/$t-*-avg.txt | gnuplot > imgs/$t.eps
#	python ./bench/plot.py $t
done 
