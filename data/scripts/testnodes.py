from pycd3 import *  # imports the CityDrain3 interface
import random as r   # used for the generating random numbers
# any other python module can be used and imported here


# this line states that the class PyMixer inherits from Node out of the pycd3 python module
class PyMixer(Node):
    # __init__ is the constructor of a python class
    # here you can add parameters, states and in and out ports
    def __init__(self):
        Node.__init__(self) # this must be called first in the constructor
# now come the parameters of the node. parameters are used for interacting with
# the outer world. They can be paths to files, double parameters like the run off coefficient
# or even lists of parameters
        self.count = Integer(2)
# do not use python integers, doubles or strings. Use the predefined Double(1.0), Integer(1),
# String("one") or Flow classes. If you need lists of values use DoubleVector, IntegerVector,
# StringVector or FlowVector
        self.addParameters()
# self.addParameters adds every predefined variable to the nodes parameter list. This can 
# also be done manually by calling self.addParameter(self.count) for example. 
        self.out = Flow()
        self.ins = []
        self.addOutPort('out', self.out)
# now come the ports of the node. Ports are for connections between other nodes. The datatype
# that can be transferred between nodes is the Flow class. It consists of a single flow and 
# one or more concentrations. One can add any number of in or out ports using self.addInPort
# or self.addOutPort. Here self.ins is a list of ports that are dynamically instantiated using
# the self.count paramter.

# The init function is the first method that is called by the citydrain framework. It allows for
# dynamic behaviours of the class like the self.ins ports. Before the init function is called 
# citydrain sets all parameter values according to the user input. so self.count now has a valid
# value. Return True if the parameters and values are accepted, return False if not. The parameter
# dialog of the GUI stays open until True is returned. Everything that gets allocated instantiated
# or added to the node MUST be deallocated deinstantiated or removed from the node in the deinit 
# function. Or else a crash is iminent. I can't catch these errors. 
# init and deinit may get called very often. After each parameter change first deinit, then init is
# called. 
    def init(self, start, stop, dt):
        for i in xrange(self.count):
            self.ins.append(Flow())
            self.addInPort('in%s' % i, self.ins[i])
        return True

# undo everything done in init (e.g. remove ports, close files, etc.)
    def deinit(self):
        for i in xrange(self.count):
            self.removeInPort('in%s' % i)

# f is the function that is called by citydrain at every timestep. before f the inports values are
# updated and you write values to the outports.
    def f(self, time, dt):
        q = 0
        for f in self.ins:
            q = q + f[0]
        self.out[0] = q
        return dt


# an empty template to start with. add it to an empty project and check where and
# when all methods are called
class Template(Node):
    def __init__(self):
        Node.__init__(self)
        print "Template.__init__()"

    def init(self, start, stop, dt):
        print "Template.init(start, stop, dt)"
        return True

    def deinit(self):
        print "Template.deinit()"

    def start(self):
        print "Template.start()"

    def stop(self):
        print "Template.stop()"

    def f(self, time, dt):
        print "Template.f(time, dt)"
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

# no need for init or deinit here everything is constant

    def f(self, time, dt):
        assign(self.out, self.const_flow)
        return dt

# a class showing how to write to a file
class PyOut(Node):
    def __init__(self):
        Node.__init__(self)
        self.out = String("/tmp/pyout.log")
        self.addParameters()
        self.in_flow = Flow()
        self.addInPort('in', self.in_flow)

    def init(self, start, stop, dt):
        log("using out name %s" % str(self.out))
        
        return True

# start and stop were specifically introduced for files. because if init and deinit are 
# more than once you would end up with empty files :-(. start and stop are called directly
# before the simulation starts and stop directly afterwards. the best place for opening and
# closing files
    def start(self):
        self.file = open(self.out.value, 'w')

    def stop(self):
        self.file.close()

    def f(self, time, dt):
        self.file.write("%s" % time.to_datetime())
        for i in xrange(len(self.in_flow)):
            self.file.write('\t%s' % self.in_flow[i])
        self.file.write('\n')
        return dt

class RandomCatchment(Node):
    def __init__(self):
        Node.__init__(self)
        self.out = Flow()
        self.addOutPort('out', self.out)
        r.seed()
        
    def f(self, time, dt):
#        print "f in time: %s" % time.to_datetime()
        self.out[0] = r.random()*30
        for i in range(len(self.out)):
            self.out[i] = r.random()
        return dt  
        
