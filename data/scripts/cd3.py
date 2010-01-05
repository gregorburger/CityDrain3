import sys
sys.path.append("build/")
sys.path.append("data/scripts")
import pycd3 as cd

class PythonNodeFactory(cd.NodeFactory):
	def __init__(self, node_klass):
		cd.NodeFactory.__init__(self)
		self.klass = node_klass
		
	def createNode(self):
		return self.klass()
		
	def getNodeName(self):
		return self.klass.__name__

def defineflow():
    fd = cd.FlowDefinition()
    fd['Q'] = cd.CalculationUnit.flow
    fd['C0'] = cd.CalculationUnit.concentration
    fd['C1'] = cd.CalculationUnit.concentration
    cd.Flow.define(fd)
    
    
def register_python_nodes(nr, module):
	__import__(module, None, None, [], 1)
	for n in cd.Node.__subclasses__():
		if nr.contains(n.__name__):
			continue
		cd.log("registering python node %s" % n.__name__)
		nr.addNodeFactory(PythonNodeFactory(n))

def setup_test_model(model, nr, simulation):
	src = nr.createNode("RandomCatchment")
	src.init(0, 7200, 300)
	src.setId('source')

	snk = nr.createNode("FileOut")
	snk.init(0, 7200, 300)
	snk.setId('sink')
	
	con = simulation.createConnection(src, 'out', snk, 'in')
	model.addNode(src)
	model.addNode(snk)
	model.addConnection(con)
	simulation.setModel(model)
	sp = cd.SimulationParameters(0, 72000, 300)
	simulation.setSimParams(sp)

def main():
    cd.init()
    defineflow()
    nr = cd.NodeRegistry()
    nr.addNativePlugin("nodes")
    sr = cd.SimulationRegistry()
    sr.addNativePlugin("nodes")
    register_python_nodes(nr, 'testnodes')
    sim = sr.createSimulation("OrderedPipeSimulation")
    model = cd.Model()
    setup_test_model(model, nr, sim)
    sim.start(0)


if __name__ == "__main__":
    main()
