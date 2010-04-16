from pycd3 import *
import random as r

CU = CalculationUnit

asdf

class PyMixer(Node):
    def __init__(self):
        Node.__init__(self)
        self.count = 2
        self.addParameters()
        self.out = Flow()
        self.ins = []
        self.addOutPort('out', self.out)
        
    def init(self, start, stop, dt):
        for i in xrange(self.count):
            self.ins.append(Flow())
            self.addInPort('in%s' % i, self.ins[i])
        return True
    
    def f(self, time, dt):
        q = 0
        for f in self.ins:
            q = q + f[0]
        self.out[0] = q
        return dt  

class PyNull(Node):
    def __init__(self):
        Node.__init__(self)
        self.out = Flow()
        self.addOutPort('out', self.out)
        self.c = 0

    def init(self, s, st, dt):
        return True

    def deinit(self):
        pass

    def f(self, time, dt):
        print "hallo %s" % self.c
        self.c += 1
        return dt
        
class PyConstFlow(Node):
    def __init__(self):
        Node.__init__(self)
        self.const_flow = Flow()
        self.addParameters()
        self.out = Flow()
        self.addOutPort('out', self.out)
        self.x = 10
       
#    def init(self, start, stop, dt):
#        print "init"
#        return True

    def deinit(self):
        print "gaga"

    def f(self, time, dt):
#        print "f"
        assign(self.out, self.const_flow)
        return dt

class PyOut(Node):
	def __init__(self):
		Node.__init__(self)
		self.out = str("/tmp/pyout.log")
		self.addParameters()
		self.in_flow = Flow()
		self.addInPort('in', self.in_flow)
		
	def init(self, start, stop, dt):
		log("using out name %s" % str(self.out))
		self.file = open(self.out, 'w')
		return True
		
		
	def f(self, time, dt):
		self.file.write("%s" % time.to_datetime())
		for i in xrange(len(self.in_flow)):
			self.file.write('\t%s' % self.in_flow[i])
		self.file.write('\n')
		return dt
		
	def __del__(self):
		self.file.close()

class RandomCatchment(Node):
    def __init__(self):
        Node.__init__(self)
        self.out = Flow()
        self.addOutPort('out', self.out)
        
    def init(self, start, stop, dt):
        r.seed()
        return True

    def deinit(self):
        pass
        
    def f(self, time, dt):
        print "f in time: %s" % time.to_datetime()
        self.out[0] = r.random()*30
        for i in range(len(self.out)):
            self.out[i] = r.random()
        return dt  
