from cd3modelgen import *

c1=ConstSource()
c2=ConstSource()
m=Mixer(2)
sew=Sewer()
spl=Splitter(0.2)
sew1=Sewer()
sew2=Sewer()
f1=FileOut("tmp/comp1-out.txt")
f2=FileOut("tmp/comp2-out.txt")

sim = Simulation("OrderedPipeSimulation")
sim.nodes += [c1, c2, m, sew, spl, sew1, sew2, f1, f2]
sim.cons += [Connection(c1, m, "out", "inputs[0]"), 
	Connection(c2, m, "out", "inputs[1]"),
	Connection(m, sew),
	Connection(sew, spl),
	Connection(spl, sew1, "out1", "in"),
	Connection(spl, sew2, "out2", "in"),
	Connection(sew1, f1),
	Connection(sew2, f2)]
	
sim.render()
