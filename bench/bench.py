#!/usr/bin/python

import sys


def main(argv):
	benchf = open(argv[1])
	values = {}
	counts = {}
	for l in benchf:
		#print l
		columns = l.split('\t')
		
		threadcount = int(columns[0])
		
		for ci in range(len(columns)-1):
			value = float(columns[ci+1])
		
			if threadcount not in values:
				values[threadcount] = (len(columns)-1)*[0]
				counts[threadcount] = 0
		
			values[threadcount][ci] = values[threadcount][ci] + value
		
		counts[threadcount] = counts[threadcount] + 1
	
#	print values[1]
#	print counts[1]
	
	for (k,v) in values.items():
		wrout =  sys.stdout.write
		wrout("%d" % k)
		for vi in v:
			wrout("\t%f" % (vi/counts[k]))
#			print "%d\t%f" % (k, v / counts[k])
		wrout("\n")
	
if __name__ == "__main__":
	main(sys.argv)
