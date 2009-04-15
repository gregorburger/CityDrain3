#!/usr/bin/env python
import sys
from cd3modelgen import *

s = Simulation("ParallelSimulation")

def gen_sewers(count = 100, subreaches = 100):
    c = ConstSource()
    s.nodes+=[c]
    old = c
    for i in xrange(count):
        sewer = Sewer(subreaches)
        s.nodes+=[sewer]
        s.cons+=[Connection(old, sewer)]
        old = sewer
    return old

def main():
    if (len(sys.argv) > 1):
        par = int(sys.argv[1])
    else:
        par = 4
    m = Mixer(par)
    for i in range(par):
        sewer = gen_sewers()
        s.cons += [Connection(sewer, m, "out", "inputs[%d]" % i)]
    f = FileOut("tmp/par-test.txt")
    s.nodes += [f, m]
    s.cons += [Connection(m, f)]
    s.render()


if __name__ == "__main__":
    main()


