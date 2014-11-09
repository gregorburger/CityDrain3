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

#include "nodeconnection.h"
#include "node.h"

NodeConnection::NodeConnection(Node * source, const std::string &soport,
							   Node *sink, const std::string &siport)
	: source(source), sink(sink), source_port(soport), sink_port(siport) {
	cd3assert(source->const_out_ports->count(source_port),
			  str(format("source node[%1%] port[%2%] not found") % source->getId() % source_port));
	cd3assert(sink->const_in_ports->count(sink_port),
			  str(format("sink node[%1%] port[%2%] not found") % sink->getId() % sink_port));

}

NodeConnection::~NodeConnection() {

}

void NodeConnection::push(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}

void NodeConnection::pull(int dt) {
	(void) dt;
	sink->setInPort(sink_port, source->getOutPort(source_port));
}
