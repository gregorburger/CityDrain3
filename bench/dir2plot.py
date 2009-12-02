#!/usr/bin/env python

header="""
	\\begin{tikzpicture}
		\\pgfplotsset{every axis legend/.append style={
				at={(0.5,1.03)},
				anchor=south}}
		\\begin{axis}[
			legend columns=-1,
			xlabel=thread count,
	%		ylabel={%s},
			grid=major,
			cycle list name=black white,
			width=0.55\\textwidth]

"""

footer="""
		\\end{axis}
	\\end{tikzpicture}

"""

from glob import glob
from sets import Set
from re import match
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

	if not len(glob("%s/*-%s-%s-time-avg.txt" %(dir, "par", pid))):
		continue


	print "\\begin{figure}[htb]"

	print "\\subfloat[runtime]{"
	print header# % "runtime $[ms]$"
	for strat in strats.keys():
		g=glob("%s/*-%s-%s-time-avg.txt" %(dir, strat, pid))
		print "		\\addplot plot file {%s};" % g[0]		
		print "		\\addlegendentry{%s}" % strats[strat]
	print footer
	print "}"
	
	print "\\subfloat[speedup]{"
	print header# % "speedup"
	for strat in strats.keys():
                g=glob("%s/*-%s-%s-time-avg.txt.speedup" %(dir, strat, pid))
                print "         \\addplot plot file {%s};" % g[0]
                print "         \\addlegendentry{%s}" % strats[strat]
	print "		\\addplot[mark=none] coordinates {(1,1)(2,2)(3,3)(4,4)(5,5)(6,6)(7,7)(8,8)(9,9)(10,10) };";
        print footer
	print "}"	#subloat


	print "\\caption{Test Figure}"
	print "\\end{figure}"
