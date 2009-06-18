import sys

w = sys.stdout.write

klass_counts = {}

class Node():
	def __init__(self, klass):
		try:
			klass_counts[klass] += 1
		except:
			klass_counts[klass] = 0
		self.klass = klass
		self.name = "%s-%d" % (self.klass, klass_counts[klass])
		self.params = []
		
	def render(self):
		self.render_start()
		self.render_contents()
		self.render_stop()
		
	def render_start(self):
		w("\n\t\t\t<node id=\"%s\" class=\"%s\">\n" % (self.name, self.klass))
	
	def render_stop(self):
		w("\t\t\t</node>\n\n")
	
	def render_contents(self):
		for param in self.params:
			if param.simple:
				w("\t\t\t\t<parameter name=\"%s\" type=\"%s\" value=\"%s\" />\n" % (param.name, param.type, str(param.value)))
			else:
				w("\t\t\t\t<parameter name=\"%s\" kind=\"complex\" type=\"%s\">\n" % (param.name, param.type))
				param.value.render()
				w("\t\t\t\t</parameter>\n")
				
class SimpleParam():
	def __init__(self, name, value):
		self.name = name
		self.type = "unknown"
		self.simple = True
		if type(value) == int:
			self.type = "int"
		if type(value) == float:
			self.type = "double"
		if type(value) == str:
			self.type = "string"
			
		if self.type == "unknown":
			raise "unknown type"
		self.value = value
		
class FlowParam():
	def __init__(self, name, flow):
		self.name = name
		self.type = "Flow"
		self.simple = False
		self.value = flow
			
		
class Sewer(Node):
	def __init__(self, N=11, K=300, X=0.1):
		Node.__init__(self, "Sewer")
		self.params += [SimpleParam("N", N), SimpleParam("K", K), SimpleParam("X", X)]
		
class ConstSource(Node):
	def __init__(self, const_flow=None):
		Node.__init__(self, "ConstSource")
		self.const_flow = const_flow
		if not self.const_flow:
			self.const_flow = Flow()
			self.const_flow.flows["Q"] = 100.0
			self.const_flow.concs["C0"] = 1.0
			self.const_flow.concs["C1"] = 5.0
			self.const_flow.concs["C2"] = 3.0
		self.params += [FlowParam("const_flow", self.const_flow)]
			
class FileOut(Node):
	def __init__(self, out_file_name="tmp/genout.txt"):
		Node.__init__(self, "FileOut")
		self.params += [SimpleParam("out_file_name", out_file_name)]
		
class Mixer(Node):
	def __init__(self, num_inputs=2):
		Node.__init__(self, "Mixer")
		self.params += [SimpleParam("num_inputs", num_inputs)]
		
class Catchment(Node):
	def __init__(self):
		Node.__init__(self, "CatchmentCSS")
		#TODO set params

class CSO(Node):
	def __init__(self):
		Node.__init__(self, "CSO")
		#TODO set params

class RainRead(Node):
	def __init__(self, file_name):
		Node.__init__(self, "RainRead")
		self.params += [SimpleParam("file_name", file_name)]

class Splitter(Node):
	def __init__(self, ratio=0.5):
		Node.__init__(self, "Splitter")
		self.params += [SimpleParam("ratio", ratio)]

class Connection():
	def __init__(self, source, sink, src_port="out", snk_port="in"):
		self.sink = sink
		self.source = source
		self.snk_port = snk_port
		self.src_port = src_port
		pass
		
	def render(self):
		self.render_start()
		self.render_contents()
		self.render_stop()
		
	def render_start(self):
		w("\n\t\t\t<connection id=\"con-%s-%s\">\n" % (self.source.name, self.sink.name))
	
	def render_stop(self):
		w("\t\t\t</connection>\n\n")
	
	def render_contents(self):
		w("\t\t\t\t<source node=\"%s\" port=\"%s\" />\n" % (self.source.name, self.src_port))
		w("\t\t\t\t<sink node=\"%s\" port=\"%s\" />\n" % (self.sink.name, self.snk_port))
		pass

class Simulation():
	def __init__(self, klass="DefaultSimulation", start=0, stop=7200, dt=300):
		self.klass = klass
		self.start = int(start)
		self.stop = int(stop)
		self.dt = int(dt)
		self.nodes = []
		self.cons = []
		
	def render(self):
		self.render_header()
		self.render_contents()
		self.render_footer()
		
	def render_contents(self):
		ren_lam = lambda r: r.render()
		w("\t<model>\n")
		w("\t\t<nodelist>\n")
		map(ren_lam, self.nodes)
		w("\t\t</nodelist>\n")
		w("\t\t<connectionlist>\n")
		map(ren_lam, self.cons)
		w("\t\t</connectionlist>\n")
		w("\t</model>\n")
		
	def render_header(self):
		w(header)
		w(sim_type % (self.klass, self.start, self.stop, self.dt))
		
	def render_footer(self):
		w("</citydrain>\n")

class Flow():
	def __init__(self):
		self.rains = {}
		self.flows = {}
		self.concs = {}
		
	def render(self):
#		w("\t\t\t\t<parameter name=\"%s\" kind=\"complex\" type=\"Flow\">\n" % self.name)
		w("\t\t\t\t\t<flow>\n")
		for key in self.rains:
			w("\t\t\t\t\t\t<unit name=\"%s\" kind=\"Rain\" value=\"%f\" />\n" %(key, self.rains[key]))
		for key in self.flows:
			w("\t\t\t\t\t\t<unit name=\"%s\" kind=\"Flow\" value=\"%f\" />\n" %(key, self.flows[key]))
		for key in self.concs:
			w("\t\t\t\t\t\t<unit name=\"%s\" kind=\"Concentration\" value=\"%f\" />\n" %(key, self.concs[key]))
		w("\t\t\t\t\t</flow>\n")
			

header = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE citydrain SYSTEM "file:../dtd/model.dtd">

<citydrain version="1.0">

	<pluginpath path="./libnodes.so" />
"""

sim_type = """	<simulation class="%s">
		<time start="%d" stop="%d" dt="%d" />
	</simulation>
"""
