from cd3modelgen import *
import sys

s = Simulation("PipelinedSimulation")

def gensewer(count):
	c = ConstSource()
	s.nodes += [c]
	old = c
	for i in xrange(count):
		sink = Sewer()
		s.nodes += [sink]
		s.cons += [Connection(old, sink)]
		old = sink
	f = FileOut("tmp/longsewerout.txt")
	s.nodes += [f]
	s.cons += [Connection(old, f)]
	
def main():
	gensewer(5000)
	s.render()
	
if __name__ == "__main__":
	main()
