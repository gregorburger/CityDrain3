#!/usr/bin/python

import sys


def main(argv):
	in_file = open(argv[1])
	res = {}
	for l in in_file:
		columns = l.split('\t')
		nt = int(columns[0])
		runtime = float(columns[1])
		if not nt in res:
			res[nt] = [runtime]
		else:
			res[nt].append(runtime)

	for (k,v) in res.items():
		print("%s\t%s" % (k, min(v)))

if __name__ == "__main__":
	main(sys.argv)
