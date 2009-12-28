import sys
sys.path.append("build/")
import pycd3 as cd

def defineflow():
    fd = cd.FlowDefinition()
    fd['Q'] = cd.CalculationUnit.flow
    fd['C0'] = cd.CalculationUnit.concentration
    fd['C1'] = cd.CalculationUnit.concentration
    cd.Flow.define(fd)

def main():
    cd.init()
    defineflow()
    nr = cd.NodeRegistry()
    nr.addNativePlugin("nodes")
    sr = cd.SimulationRegistry()
    sr.addNativePlugin("nodes")
    
    sim = sr.createSimulation("DefaultSimulation")
    model = cd.Model()
    
    const_source = nr.createNode("ConstSource")

if __name__ == "__main__":
    main()
