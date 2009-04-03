#!/usr/bin/env python

import sys

node_templ = """			<node id="musk-%d-%d" class="Sewer" />"""
sewer_con_templ = """			
			<connection id="sewer-con-%d-%d">
				<source node="musk-%d-%d" port="out" />
				<sink node="musk-%d-%d" port="in" />
			</connection>
"""

header_templ = """<?xml version="1.0"?>
<!DOCTYPE citydrain SYSTEM "model.dtd">

<citydrain version="1.0">

	<pluginpath path="./libnodes.so" />

	<simulation class="DefaultSimulation">
		<time start="0" stop="72000" dt="300" />
	</simulation>

	<model>
		<nodelist>
"""

nodes_cons_seperator_templ = """
		</nodelist>
		
		<connectionlist>
"""
footer_templ = """
			
		</connectionlist>
	</model>
</citydrain>
"""

const_src_templ = """
			<node id="constsource-%d" class="ConstSource">
				<parameter name="const_flow" kind="complex" type="Flow">
					<flow>
						<unit name="Q" kind="Flow" value="234.0" />
						<unit name="C0" kind="Concentration" value="0.1"/>
						<unit name="C1" kind="Concentration" value="0.2"/>
					</flow>
				</parameter>
			</node>
"""

file_out_templ = """
			<node id="fileout" class="FileOut">
				<parameter name="out_file_name" type="string" value="tmp/par-sewer.txt" />
			</node>
"""

const_sewer_con_templ = """
			<connection id="const-sewer-%d">
				<source node="constsource-%d" port="out" />
				<sink	node="musk-%d-0" port="in" />
			</connection>
"""

mixer_templ = """
			<node id="mixer" class="Mixer">
				<parameter name="num_inputs" type="int" value="%d" />
			</node>
"""

sewer_mixer_con_templ = """
			<connection id="sewer-mixer-%d">
				<source node="musk-%d-%d" port="out" />
				<sink	node="mixer" port="inputs[%d]" />
			</connection>
"""

fileout_con_templ = """
			<connection id="mixer-fileout">
				<source node="mixer" port="out" />
				<sink	node="fileout" port="in" />
			</connection>
"""

def make_nodes(ns, np):
	for p in xrange(np):
		print const_src_templ % (p)
		for s in xrange(ns):
			print node_templ % (p, s)
	print mixer_templ % np
	print file_out_templ
		
def make_cons(ns, np):
	for p in xrange(np):
		print const_sewer_con_templ % (p, p, p)
		for s in xrange(ns-1):
			print sewer_con_templ % (p, s, p, s, p, s+1)
		print sewer_mixer_con_templ % (p, p, s+1, p)
	print fileout_con_templ
	

if __name__ == "__main__":
	if (len(sys.argv) < 3):
		sys.stderr.write("useage: %s ns np\n" % sys.argv[0])
		exit(-1)
	print header_templ
	ns = int(sys.argv[1])
	np = int(sys.argv[2])
	make_nodes(ns, np)
	print nodes_cons_seperator_templ
	make_cons(ns, np)
	print footer_templ
