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
rc('figure.subplot', left=0.185, right=0.985, top=0.97, bottom=0.15)

model = sys.argv[1]
datas={}

sim=['ordered', 'par', 'pipe']

#g = glob.glob('time/%s-*-avg.txt' % model)
#g.sort()

for s in sim:
	strat = glob.glob('time/%s-%s-*-avg.txt' % (model, s))[0]
	datas[s] = csv2rec(strat, delimiter="\t", names=['nthreads', 'runtime'])
	
x=[]
for s in sim:
	x.append(datas[s].runtime)

f=figure(figsize=(8/inch, 5.1/inch))
	
lines=plot(transpose(x))
ls = ['-', '--', ':']
ymax=0
for i in range(3):
	lines[i].set_linestyle(ls[i])
	ymax=max(ymax, lines[i].get_ydata()[0])
	
ylim(ymax=ymax*1.2, ymin=0)
xlabel('number of threads')#, va='bottom')
ylabel('runtime (ms)')#, ha='left')
l=legend(lines, ['ordered', 'parallel', 'pipelined'], 'best')
#l=legend(lines, datas.keys(), 'best')
l.draw_frame(False)
savefig('imgs/%s.%s' % (model, format), format=format, dpi=400)#, bbox_inches="tight")
#show()

