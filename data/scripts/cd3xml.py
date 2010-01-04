from xml.sax.handler import ContentHandler
from xml.sax.xmlreader import XMLReader
from xml.sax import parse
import sys
sys.path.append('./build/')
from pycd3 import *
from cd3 import register_python_nodes

class Handler(ContentHandler):
	def __init__(self):
		self.model = Model()
		self.node_reg = NodeRegistry()
		self.sim_reg = SimulationRegistry()
		
		
	def startElement(self, name, attrs):
		if name == 'pluginpath':
			self.node_reg.addNativePlugin(str(attrs['path']))
			self.sim_reg.addNativePlugin(str(attrs['path']))
			self.fd = None
			return
			
		if name == 'pythonmodule':
			register_python_nodes(self.node_reg, str(attrs['module']))
			return
			
		if name == 'simulation':
			self.simulation = self.sim_reg.createSimulation(str(attrs['class']))
			return
		
		if name == 'time':
			self.sp = SimulationParameters(int(attrs['start']), int(attrs['stop']), int(attrs['dt']))
			self.simulation.setSimulationParameters(self.sp)
			return
			
		if name == 'flowdefinition':
			self.fd = FlowDefinition()
			return
			
		if name == 'flow' and self.fd != None:
			self.fd[str(attrs['name'])] = CalculationUnit.flow
			return
			
		if name == 'concentration' and self.fd != None:
			self.fd[str(attrs['name'])] = CalculationUnit.concentration
			return
			
		if name == 'node':
			self.current_node = self.node_reg.createNode(str(attrs['class']))
			self.current_node.setId(str(attrs['id']))
			return
			
		if name == 'parameter':
			log("parameter loading is unimplemented", LogLevel.error)
			return
		
		
		if name == 'source':
			self.source = (attrs['node'], attrs['port'])
			return
		
		if name == 'sink':
			self.sink = (attrs['node'], attrs['port'])
			return 
		ignored = ['connection', 'connectionlist', 'model', 'nodelist', 'citydrain']
		if name not in ignored:
			log(str("unknown tag: %s" % name), LogLevel.warning)
	def endElement(self, name):
		if name == 'flowdefinition':
			Flow.define(self.fd)
			self.fd = None
			
		if name == 'node':
			self.current_node.init(self.sp.start, self.sp.stop, self.sp.dt)
			self.model.addNode(self.current_node)
			self.current_node = None
			
		if name == 'connection':
			src_node = self.model.getNode(str(self.source[0]))
			snk_node = self.model.getNode(str(self.sink[0]))
			con = self.simulation.createConnection(src_node, str(self.source[1]), snk_node, str(self.sink[1]))
			self.model.addConnection(con)

class ProgressHandler():
    def __init__(self):
        self.first = True
        self.perc = 0
        
    def __call__(self, sim, time):
        if self.first:
            self.params = sim.getSimulationParameters()
            self.first = False
            self.duration = float(self.params.stop - self.params.start)
        newperc = int(time / self.duration * 100)
        if self.perc != newperc:
            self.perc = newperc
            log("progress:\t%d" % self.perc)

if __name__ == "__main__":
	if len(sys.argv) <= 1:
		print "provide model file"
		sys.exit(-1)
		
	init()
	
	handler = Handler()
	parse(sys.argv[1], handler)
	
	handler.simulation.setModel(handler.model)
   	handler.simulation.timestep_after(ProgressHandler())
	handler.simulation.start(0)
