import pycd3

class PythonNode(pycd3.Node):
	def __init__(self):
		pycd3.Node.__init__(self)
		self.x = 10
#		self.addParameter('x', self.x)
		self.flow = pycd3.Flow()
		self.addInPort('in', self.flow)
		self.addOutPort('out', self.flow)
		pass
	
	def f(self, time, dt):
		return dt

class MichiPythonNode(pycd3.Node):
	def __init__(self):
		pycd3.Node.__init__(self)
		self.flow = pycd3.Flow()
		self.addInPort('in', self.flow)
		self.addOutPort('out', self.flow)
		self.x = 10
		self.addParameters()
	
	def init(self, start, stop, dt):
	    pass
		#print "init"
	
	def f(self, time, dt):
#		print "f: %s - %s" % (time, dt)
		#print self.x
		return dt

class DPythonNode(pycd3.Node):
	def __init__(self):
		pycd3.Node.__init__(self)
		f = pycd3.Flow()
		pycd3.test_flow(f)
		#self.addInPort("in", f)
#		fout = pycd3.Flow()
		#self.addOutPort("out", f)
		print "ende __init__"
		
	def f(self, t, dt):
		print 'f'
		
	def init(self, start, stop, dt):
		print 'init'
		return True
		
		
		
