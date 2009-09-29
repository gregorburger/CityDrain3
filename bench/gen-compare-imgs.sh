#!/bin/bash

for test in `ls data/models/single/`
do
	./bench/compare-job.sh ${test%.xml}
done
