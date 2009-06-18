#!/usr/bin/env python

from cd3modelgen import *

s = Simulation()
c = ConstSource()
m = Mixer()
s1=Sewer()
s2=Sewer()
sp = Splitter();
f = FileOut()

s.nodes += [c, m, sp, f, s1, s2]
s.cons += [	Connection(c, m, snk_port = "inputs[0]"),
	Connection(m, sp),
	Connection(sp, s1, "out1", "in"),
	Connection(sp, s2, "out2", "in"),
	Connection(s2, m, "out", "inputs[1]"),
	Connection(s1, f)]
s.render()
