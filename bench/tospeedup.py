#!/usr/bin/python

from pylab import *
import sys

csv = csv2rec(sys.stdin, delimiter="\t", names=['nthreads', 'speedup'])
csv.speedup = map(lambda r: csv.speedup[0] / r, csv.speedup)
#rec2csv(csv, sys.stdout, delimiter="\t")
savetxt(sys.stdout, csv, delimiter="\t", fmt=("%d","%f"))
