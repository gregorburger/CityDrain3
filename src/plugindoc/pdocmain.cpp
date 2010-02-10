#include <boost/foreach.hpp>
#include <string>
using namespace std;

#include <noderegistry.h>
#include <simulationregistry.h>
#include <node.h>
#include <flow.h>
#include <log.h>
#include <logsink.h>
#include <sstream>

typedef pair<string, ltvp> par_type;
typedef pair<string, Flow*> port_type;
typedef pair<string, NodeParameter*> param_pair;

int main(int argc, char **argv) {
	ostringstream out;
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
	cout << "Nodes: ";
	BOOST_FOREACH(string klass, r.getRegisteredNames()) {
		Node *node = r.createNode(klass);
		cout << endl << klass << ":" << endl;

		BOOST_FOREACH(param_pair par, node->getParameters()) {
			NodeParameter *p = par.second;
			cout << "\tparameter: " << p->name << endl;
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
//		delete node;
	}
	cout << endl << "Simulations: " << endl;
	BOOST_FOREACH(string klass, sr.getRegisteredNames()) {
		cout << "\t" << klass << endl;
	}
	Log::shutDown();
	return 0;
}
