#!/usr/bin/env python
from xml.sax import *
import sys

class ModelHandler(ContentHandler):
	def __init__(self):
		self.nodes = {}
		self.cons = []
		
	def startElement(self, name, attrs):
		if (name == "node"):
			self.nodes[attrs.get("id")] = attrs.get("class")
		if (name == "sink"):
			sink_node = attrs.get("node")
			self.cons += [(self.source_node, sink_node)]
		if (name == "source"):
			self.source_node = attrs.get("node")
			
	def render(self):
		print "digraph G {"
		print "graph[rankdir=LR,pagedir=LT]"
		print "node[shape=rect]"
		for n in self.nodes:
			print """"%s"[label="%s"]""" % (n, self.nodes[n])
		print
		for c in self.cons:
			print """ "%s" -> "%s" """ % c
		print "}"
		
def main():
	mh = ModelHandler()
	if len(sys.argv) > 1:
		parse(sys.argv[1], mh)
	else:
		parse(sys.stdin, mh)
	mh.render()

if __name__ == "__main__":
	main()
