from pylab import *
import glob
import sys

inch=2.54

if len(sys.argv) < 2:
	print "usage: plot.py model"
	exit(-1)

format="png"
	
if len(sys.argv) > 2:
	format=sys.argv[2]

rc('font', size=8)
rc('figure.subplot', left=0.17, right=0.985, top=0.97, bottom=0.15)

model = sys.argv[1]
datas={}

g = glob.glob('%s-*-avg.txt' % model)
g.sort()

for strat in g:
	datas[strat] = csv2rec(strat, delimiter="\t", names=['nthreads', 'runtime'])
	
x=[]
for d in datas:
	x.append(datas[d].runtime)

#subplots_adjust(right=0.7)

f=figure(figsize=(8/inch, 5.1/inch))
#f=figure(figsize=(5, 3))
	
lines=plot(transpose(x))
ls = ['-', '--', ':']
for i in range(3):
	lines[i].set_linestyle(ls[i])
#	lines[i].set_linewidth(2)
	
#for l in lines:
#	base=l.get_ydata()[0]
#	for xyi in xrange(1, len(l.get_xydata())):
#		xy = l.get_xydata()[xyi]
#		text(xy[0], xy[1], "%.1f" % float(base/xy[1]), ha="center", va="baseline", size=10)

#axis('off')

xlabel('number of threads')#, va='bottom')
ylabel('runtime (ms)')#, ha='left')
l=legend(lines, ['ordered', 'parallel', 'pipelined'], 'best')
l.draw_frame(False)
savefig('%s.%s' % (model, format), format=format, dpi=400)#, bbox_inches="tight")
#show()

