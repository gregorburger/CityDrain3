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

f = pycd3.Flow()
print "len of of f == %s" % (len(f))
f[0] = 3.142
print "getith %s" % f.getIth(pycd3.CalculationUnit.flow, 0)
print f[0]
print "the flows names are: %s" % pycd3.Flow.names()
print "the value of Q is: %s" % f.getValue('Q')
f.setValue('Q', 42.0)
print "the value of Q is: %s" % f.getValue('Q')
f.clear()
print "the value of Q is: %s" % f.getValue('Q')
for n in pycd3.Flow.names():
    print "Unit of %s is: %s" % (n, f.getUnit(n))
    
f.dump()

