#!/usr/bin/env python
import sys

def sumQ(path):
    f = open(path, 'r')
    s = 0.0;
    for line in f:
        try:
            s += float(line.split()[2])
        except:
            pass
    f.close()
    return s


if __name__ == "__main__":
    for path in sys.argv[1:]:
        print "Qsum for %s is \t\t\t%s" % (path, sumQ(path))
