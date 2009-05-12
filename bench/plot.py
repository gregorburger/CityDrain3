from pylab import *
import glob
import sys

if len(sys.argv) < 2:
	print "usage: plot.py model"
	exit(-1)

model = sys.argv[1]
datas={}

for strat in glob.glob('%s-*-avg.txt' % model):
	datas[strat] = csv2rec(strat, delimiter="\t", names=['nthreads', 'runtime'])
	
x=[]
for d in datas:
	x.append(datas[d].runtime)
	
lines=plot(transpose(x))
ls = ['-', '--', ':']
for i in range(3):
	lines[i].set_linestyle(ls[i])
#	lines[i].set_linewidth(2)
	
for l in lines:
	base=l.get_ydata()[0]
	for xyi in xrange(1, len(l.get_xydata())):
		xy = l.get_xydata()[xyi]
		text(xy[0], xy[1], "%.1f" % float(base/xy[1]), ha="center", va="baseline")

#axis('off')
xlabel('number of threads')
ylabel('runtime')
savefig('%s.pdf' % model, format='pdf')
#show()

