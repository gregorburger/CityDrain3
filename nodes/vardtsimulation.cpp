#include "vardtsimulation.h"
#include <node.h>
#include <model.h>
#include <flowbuffer.h>
#include <flow.h>
#include <cd3assert.h>


#include <boost/foreach.hpp>
#include <boost/tuple/tuple_comparison.hpp>

using namespace boost::tuples;

CD3_DECLARE_SIMULATION_NAME(VarDTSimulation)

typedef tuple<Node *, std::string, Node *, std::string> con_type;
typedef std::map<con_type, FlowBuffer *> buf_type;

struct VarDTPriv {
	buf_type buffer;
};

VarDTSimulation::VarDTSimulation() {
	priv = new VarDTPriv();
}

VarDTSimulation::~VarDTSimulation() {
	delete priv;
}

/*void VarDTSimulation::addController(IController *c) {
	(void) c;
}*/

int VarDTSimulation::run(int time, int dt) {
	cd3assert(model->getSinkNodes().size() == 1,
		   "can not handle more than one sink node aka there is only one see");
	run(*model->getSinkNodes().begin(), time, sim_param.dt);
	return dt;
}



int VarDTSimulation::run(Node *n, int time, int dt) {
	BOOST_FOREACH(next_node_type nn, model->backward(n)) {
		int calced_dt = 0;	//holds the calculated dts of the predecessors
		std::string src_port, snk_port;
		Node *next;

		tie(src_port, next, snk_port) = nn;

		con_type connection(next, src_port, n, snk_port);

		FlowBuffer *fb;

		if (priv->buffer.find(connection) == priv->buffer.end()) {
			fb = new FlowBuffer();
			priv->buffer[connection] = fb;
		} else {
			fb = priv->buffer[connection];
		}

		while (calced_dt < dt) {
			int new_dt = run(next, time, dt);
			fb->put(*next->getOutPort(src_port), new_dt);
			calced_dt += new_dt;
		}
		Flow f = fb->take(dt);
		n->setInPort(snk_port, &f);
		cd3assert(fb->buffered() == 0, "connection buffer not empty");
	}
	return n->f(time, dt);
}
