import sys
sys.path.append("/home/gregor/Work/cd3-1/build/src/python-swig/")
sys.path.append("/home/gregor/Work/cd3-1/build/")
import pycd3

class NodeFactory(pycd3.INodeFactory):
	def __init__(self, node):
		pycd3.INodeFactory.__init__(self)
		self.node = node
		print "NodeFactory.__init__"
		
	def getNodeName(self):
		print "NodeFactory.getName"
		return self.node.__name__
		
	def createNode(self):
		print "NodeFactory.createNode"
		n = self.node()
		n.__disown__()
		print "NodeFactory.disowned"
		return n
		
	def getSource(self):
		print "NodeFactory.getSource"
		return "testnodes.py"

class PythonTestNode(pycd3.Node):
	def __init__(self):
		pycd3.Node.__init__(self)
		self.out = pycd3.Flow()
		self.inf = pycd3.Flow()
		self.addOutPort("out", self.out)
		self.addInPort("in", self.inf)
		
	def init(self, start, stop, dt):
		print start
		print stop
		print dt
		return True
		
	def f(self, current, dt):
		return dt
	
	def getClassName(self):
		print "getClassName"
		return "TestNode"

def register(nr):
	for c in pycd3.Node.__subclasses__():
		nf = NodeFactory(c)
		nf.__disown__()
		nr.addNodeFactory(nf)
		
def test():
	nr = pycd3.NodeRegistry()
	nf = NodeFactory(PythonTestNode).__disown__()
	nr.addNodeFactory(nf)
	node = nr.createNode("PythonTestNode")
	
#test()
