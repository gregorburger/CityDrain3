from cd3modelgen import *
import sys

s = Simulation("PipelinedSimulation")

def gensewer(sink, c):
	source = Sewer()
	s.nodes += [source]
	s.cons += [Connection(source, sink)]
	if (c != 0):
		gensewer(source, c - 1)
	else:
		c = ConstSource()
		s.nodes += [c]
		s.cons += [Connection(c, source)]

def main():
	f = FileOut("tmp/longsewerout.txt")
	gensewer(f, 100)
	s.nodes += [f]
	s.render()

if __name__ == "__main__":
	main()
