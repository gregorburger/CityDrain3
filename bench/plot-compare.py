#!/usr/bin/env python
import matplotlib as mpl
mpl.use('cairo')

from pylab import *
import glob
import sys
import os
import re

inch=2.54

if len(sys.argv) < 3:
	print "usage plot.py model pid"
	exit(-1)
	
format="pdf"
	
if len(sys.argv) > 3:
	format=sys.argv[3]

rc('font', size=8)
rc('figure.subplot', left=0.20, right=0.95, top=0.92, bottom=0.20)

model = sys.argv[1]
pid = sys.argv[2]

f=figure(figsize=(8/inch, 5.1/inch))

ls = ['-', '--', ':']
lsi = 0
ymax=0
lines=[]
lines_names=[]
for path in os.listdir('time/'):
	rexp = "%s-([a-x]+)-%s-time-avg\.txt" % (model, pid)
	m = re.match(rexp, path)
	if (m):
		strat = "time/%s" % m.group(0)
		#datas[m.group(1)] = csv2rec(strat, delimiter="\t", names=['nthreads', 'runtime'])
		csv = csv2rec(strat, delimiter="\t", names=['nthreads', 'runtime'])
		name = m.group(1)
		line=plot(csv.nthreads, csv.runtime)
		line[0].set_linestyle(ls[lsi])
		lines.append(line)
		lines_names.append(name)
		lsi+=1
	
xlabel('number of threads')#, va='bottom')
ylabel('runtime (ms)')#, ha='left')
l=legend(lines, lines_names, 'best')
l.draw_frame(False)
savefig('imgs/%s-+%s-comp.%s' % (model, pid, format), format=format, dpi=400)#, bbox_inches="tight")
#show()

