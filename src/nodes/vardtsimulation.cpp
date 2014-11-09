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

#include "vardtsimulation.h"
#include <node.h>
#include <imodel.h>
#include <flowbuffer.h>
#include <flow.h>
#include <cd3assert.h>
#include <nodeconnection.h>


#include <boost/foreach.hpp>
#include <boost/tuple/tuple_comparison.hpp>

CD3_DECLARE_SIMULATION_NAME(VarDTSimulation)

typedef boost::tuple<Node *, std::string, Node *, std::string> con_type;
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

int VarDTSimulation::run(ptime time, int dt) {
	cd3assert(model->getSinkNodes().size() == 1,
		   "can not handle more than one sink node aka there is only one see");
	run(*model->getSinkNodes().begin(), time, sim_param.dt);
	return dt;
}



int VarDTSimulation::run(Node *n, ptime time, int dt) {
	BOOST_FOREACH(NodeConnection *con, model->backwardConnection(n)) {
		int calced_dt = 0;	//holds the calculated dts of the predecessors
		std::string src_port, snk_port;
		Node *next;

		next = con->sink;
		src_port = con->source_port;
		snk_port = con->sink_port;

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
