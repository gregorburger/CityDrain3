import cd3

class PythonNode(cd3.Node):
	def f(self, t, dt):
		print '%s, %s' %(t, dt)
		
	def init(self, start, stop):
		print "init"
		
		
		
		
