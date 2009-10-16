import cd3

class PythonNode(cd3.Node):
	def __init__(self):
		cd3.Node.__init__(self)
		self.x = 10
		self.addParameter('x', self.x)
		self.flow = cd3.Flow()
		print "ASDFAS mcib"
		self.addInPort('in', self.flow)
		self.addOutPort('out', self.flow)
		pass
	
	def init(self, start, stop, dt):
		print "init"
	
	def f(self, time, dt):
#		print "f: %s - %s" % (time, dt)
		return dt

class MichiPythonNode(cd3.Node):
	def __init__(self):
		cd3.Node.__init__(self)
		self.flow = cd3.Flow()
		self.addInPort('in', self.flow)
		self.addOutPort('out', self.flow)
		self.x = 10
		self.addParameter(self.x)
	
	def init(self, start, stop, dt):
		print "init"
	
	def f(self, time, dt):
#		print "f: %s - %s" % (time, dt)
		return dt

class DPythonNode(cd3.Node):
	def __init__(self):
		cd3.Node.__init__(self)
		f = cd3.Flow()
		cd3.test_flow(f)
		#self.addInPort("in", f)
#		fout = cd3.Flow()
		#self.addOutPort("out", f)
		print "ende __init__"
		
	def f(self, t, dt):
		print 'f'
		
	def init(self, start, stop, dt):
		print 'init'
		
		
		
		
