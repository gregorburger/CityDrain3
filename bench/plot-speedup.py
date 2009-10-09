#!/usr/bin/env python
import matplotlib as mpl
mpl.use('cairo')

from pylab import *
import glob
import sys

inch=2.54

if len(sys.argv) < 3:
	print "usage: plot.py model pid"
	exit(-1)

format="pdf"
	
#if len(sys.argv) > 3:
#	format=sys.argv[3]

rc('font', size=8)
#rc('figure.subplot', left=0.13, right=0.97, top=0.97, bottom=0.18)
rc('figure.subplot', left=0.13, right=0.95, top=0.92, bottom=0.19)

model = sys.argv[1]
pid = sys.argv[2]

sim=['ordered', 'par', 'pipe']
appr_legend={'ordered':'OPS', 'par':'FPS', 'pipe':'PPS'}

f=figure(figsize=(8/inch, 5.1/inch))
ls = ['-', '--', ':']
lsi = 0
ymax=0
lines=[]
lines_names=[]

nthreads = 0

for s in sim:
	strat = 'time/%s-%s-%s-time-avg.txt' % (model, s, pid)
	csv = csv2rec(strat, delimiter="\t", names=['nthreads', 'speedup'])
	nthreads = len(csv.nthreads)
	csv.speedup = map(lambda r: csv.speedup[0] / r, csv.speedup)
	line=plot(csv.nthreads, csv.speedup)
	line[0].set_linestyle(ls[lsi])
	lines.append(line)
	lines_names.append(appr_legend[s])
	lsi+=1	
	ymax=max(ymax, line[0].get_ydata()[0])

#ylim(ymax=ymax*1.2, ymin=0)
#ylim(0, nthreads)
#xlim(0, nthreads)
plot(range(nthreads), range(nthreads))
xlabel('number of threads')#, va='bottom')
ylabel('speedup')#, ha='left')
l=legend(lines, lines_names, 'best')
l.draw_frame(False)
savefig('imgs/%s-+%s-speedup.%s' % (model, pid, format), format=format, dpi=400)#, bbox_inches="tight")
#show()
