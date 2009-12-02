#!/usr/bin/env python
from glob import glob
from sets import Set
from re import match
from os.path import basename
import os
import sys


dir = sys.argv[1]

avgs=glob("%s/*-time-avg.txt" % dir)
pids = Set()

for avg in avgs:
	m = match(".*-([0-9]+)-time-avg.txt", avg)
	if m == None:
		continue
	pids.add(m.group(1))

strats={"par": "FPS", "ordered": "OPS", "pipe" : "PPS"}

for pid in pids:
	g = glob("%s/*-par-%s-time-avg.txt" %(dir, pid))
	if len(g) == 0:
		continue
	fname = basename(g[0])		
	m = match("(.*)-par-%s-time-avg.txt" % pid, fname)
	model = m.groups()[0]
	print "%s - %s" % (model, pid)
	
	ppid = os.fork()
	
	if ppid == 0:
		#child
		print "execing"
		os.execlp('./bench/plot-speedup.py', './bench/plot-speedup.py', model, str(pid))
		exit()
	else:
		os.waitpid(ppid, 0)
