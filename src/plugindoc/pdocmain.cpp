/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include <boost/foreach.hpp>
#include <string>
using namespace std;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <node.h>
#include <flow.h>
#include <log.h>
#include <logsink.h>
#include <typeconverter.h>
#include <sstream>

typedef pair<string, ltvp> par_type;
typedef pair<string, Flow*> port_type;
typedef pair<string, NodeParameter*> param_pair;

int main(int argc, char **argv) {
	ostringstream out;
	
	//init CityDrain3 runtime
	
	Log::init(new OStreamLogSink(out));
	
	map<string, Flow::CalculationUnit> fd;
	fd["Q"] = Flow::flow;
	fd["C"] = Flow::concentration;
	Flow::define(fd);
	
	NodeRegistry r;
	SimulationRegistry sr;
	
	for (int i = 1; i < argc; i++) {
		r.addNativePlugin(argv[i]);
		sr.addNativePlugin(argv[i]);
	}
	
	cout << "# Node Reference";
	BOOST_FOREACH(string klass, r.getRegisteredNames()) {
		
		// Do not document internal nodes. These ones should not be used
		// directly
		if (klass == "CycleNodeStart" || klass == "CycleNodeEnd") {
			continue;
		}
		
		Node *node = r.createNode(klass);
		cout << endl << "## " << klass << "##" << endl;

		cout << node->getDescription() << endl << endl;
		
		if (node->getParameters().size()) {
			cout << "### Parameters ###" << endl << endl;
	
			BOOST_FOREACH(param_pair par, node->getParameters()) {
				NodeParameter *p = par.second;
				std::string initial_value = TypeConverter::get(p->type)->toString(p->value);
				cout << "- " << p->name << "[" << p->unit << "] = " << initial_value 
				     << " : " << p->description << endl;
			}
		}

		if (node->const_states->size()) {
			cout << endl << "### States ###" << endl << endl;
	
			BOOST_FOREACH(par_type state, *node->const_states) {
				cout << "- " << state.first << endl;
			}
		}

		if (node->const_in_ports->size()) {
			cout << endl << "### In Ports ###" << endl << endl;
	
			BOOST_FOREACH(port_type port, *node->const_in_ports) {
				cout << "- " << port.first << endl;
			}
		}

		if (node->const_in_ports->size()) {
			cout << endl << "### Out Ports ###" << endl << endl;
			
			BOOST_FOREACH(port_type port, *node->const_out_ports) {
				cout << "- " << port.first << endl;
			}
		}
//		delete node;
	}
	cout << endl << "# Simulations " << endl;
	BOOST_FOREACH(string klass, sr.getRegisteredNames()) {
		cout << "## " << klass << endl;
	}
	Log::shutDown();
	return 0;
}
