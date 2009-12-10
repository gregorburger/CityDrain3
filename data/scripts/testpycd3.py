import pycd3

class TestNode(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.x = 10
        self.y = 11
        self.s = "asdf"
        self.d = 1.0
        self.n = 10
        self.addParameters()
        
    def f(self, time, dt):
        print "f"
        print self.x
        return dt
        
    def init(self, start, end, dt):
        print "init"
        print self.x
        pass 



pycd3.init()
fd = pycd3.FlowDefinition()
fd['Q'] = pycd3.CalculationUnit.flow
fd['C0'] = pycd3.CalculationUnit.concentration
fd['C1'] = pycd3.CalculationUnit.concentration
pycd3.Flow.define(fd)

n = TestNode()
pycd3.test_node(n)
