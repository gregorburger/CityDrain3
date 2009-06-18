from cd3modelgen import *

s=Simulation("VarDTSimulation")

cs=ConstSource()
sewer=Sewer()
out=FileOut("/tmp/test.txt")

s.nodes += [cs, sewer, out]
s.cons  += [Connection(cs, sewer), Connection(sewer, out)]

s.render()
