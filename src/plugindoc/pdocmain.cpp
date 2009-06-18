#include <iostream>
#include <boost/foreach.hpp>
#include <string>
using namespace std;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <node.h>
#include <flow.h>
#include <log.h>
#include <sstream>

typedef pair<string, ltvp> par_type;
typedef pair<string, Flow*> port_type;

int main(int argc, char **argv) {
	ostringstream out;
	Log::init(&out);
	NodeRegistry r;
	SimulationRegistry sr;
	for (int i = 1; i < argc; i++) {
		r.addPlugin(argv[i]);
		sr.addPlugin(argv[i]);
	}
	cout << "Nodes: ";
	BOOST_FOREACH(string klass, r.getRegisteredNames()) {
		if (klass == "QSWNode") {
			//cout << "ignoring " << klass << endl;
			continue;
		}

		Node *node = r.createNode(klass);
		if (klass != "RainRead")
			node->init(0, 7200, 300);
		cout << endl << klass << ":" << endl;

		BOOST_FOREACH(par_type par, *node->const_parameters) {
			cout << "\tparameter: " << par.first << endl;
		}

		BOOST_FOREACH(par_type state, *node->const_states) {
			cout << "\tstate: " << state.first << endl;
		}

		BOOST_FOREACH(port_type port, *node->const_in_ports) {
			cout << "\tin_port: " << port.first << endl;
		}

		BOOST_FOREACH(port_type port, *node->const_out_ports) {
			cout << "\tout_port: " << port.first << endl;
		}
		delete node;
	}
	cout << endl << "Simulations: " << endl;
	BOOST_FOREACH(string klass, sr.getRegisteredNames()) {
		cout << "\t" << klass << endl;
	}
	return 0;
}
