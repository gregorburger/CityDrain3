import pycd3

pycd3.init()
fd = pycd3.FlowDefinition()
fd['Q'] = pycd3.CalculationUnit.flow
fd['C0'] = pycd3.CalculationUnit.concentration
fd['C1'] = pycd3.CalculationUnit.concentration
pycd3.Flow.define(fd)
f = pycd3.Flow()
pycd3.test_flow(f)

