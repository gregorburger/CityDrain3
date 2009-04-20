#!/usr/bin/env python
from cd3modelgen import *
import sys

def gensewer(s, count):
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
    if (len(sys.argv) < 2):
        sys.stderr.write("usage: gen-long-sewer length [sim-class]\n")
        return
    length = int(sys.argv[1])
    s = Simulation("PipelinedSimulation")
    if (len(sys.argv) > 2):
        s = Simulation(sys.argv[2])
    gensewer(s, length)
    s.render()
    
if __name__ == "__main__":
	main()
