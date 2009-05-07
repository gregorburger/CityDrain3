#!/bin/bash

tests="tree-2 tree-4 par-4-100"

for t in $tests
do
	./bench/timejob.sh $t
done 
