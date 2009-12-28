from pycd3 import *
import random as r

CU = CalculationUnit

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
    
    def f(self, time, dt):
        q = 0
        for f in self.ins:
            q = q + f[0]
        self.out[0] = q
        return dt  

class RandomCatchment(Node):
    def __init__(self):
        Node.__init__(self)
        self.out = Flow()
        self.addOutPort('out', self.out)        
        
    def init(self, start, stop, dt):
        r.seed()
        pass
    
    def f(self, time, dt):
        for n in self.out.names():
            if Flow.getUnit(n) == CU.flow:
                self.out.setValue(n, r.random()*30)
            else:
                self.out.setValue(n, r.random())
        return dt  
      
