#!/usr/bin/env python
from cd3modelgen import *


c1=ConstSource()
c2=ConstSource()

s1=Sewer()
s2=Sewer()

m1=Mixer()

f=FileOut("tmp/test-gen-out.txt")

s=Simulation()
s.nodes+=[s1, s2, c1, c2, m1, f]
s.cons+=[Connection(c1, s1), \
	Connection(c2, s2), \
	Connection(c1, m1, "out", "inputs[0]"), \
	Connection(c2, m1, "out", "inputs[1]"), \
	Connection(m1, f)]

s.render()

