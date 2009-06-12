#!/usr/bin/env python

from cd3modelgen import *

s = Simulation()
c = ConstSource()
m = Mixer()
sp = Splitter();
f = FileOut()

s.nodes += [c, m, sp, f]
s.cons += [	Connection(c, m, snk_port = "inputs[0]"),
	Connection(m, sp),
	Connection(sp, m, "out1", "inputs[1]"),
	Connection(sp, f, "out2", "in")]
s.render()
