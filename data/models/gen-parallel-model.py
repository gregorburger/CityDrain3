#!/usr/bin/env python
import sys
from cd3modelgen import *

s = Simulation("ParallelSimulation")

def gen_sewers(s, count, subreaches):
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
    par = seq = subs = 0
    s = None
    if (len(sys.argv) < 3):
        sys.stderr.write("usage: gen-parallel-model num-par num-seq [sim-class] [sub-reaches]\n")
        return
    if (len(sys.argv) >= 3):
        par = int(sys.argv[1])
        seq = int(sys.argv[2])
    if (len(sys.argv) >= 4):
        s = Simulation(sys.argv[3])
    else:
        s = Simulation("ParallelSimulation")
    if (len(sys.argv) >= 5):
        subs = int(sys.argv[4])
    else:
        subs = 11
    m = Mixer(par)
    for i in range(par):
        sewer = gen_sewers(s, seq, subs)
        s.cons += [Connection(sewer, m, "out", "inputs[%d]" % i)]
    f = FileOut("tmp/par-test.txt")
    s.nodes += [f, m]
    s.cons += [Connection(m, f)]
    s.render()


if __name__ == "__main__":
    main()


