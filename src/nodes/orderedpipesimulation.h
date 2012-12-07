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

#ifndef ORDEREDPIPESIMULATION_H
#define ORDEREDPIPESIMULATION_H

#include <vector>

#include <simulation.h>
#include <boost/shared_ptr.hpp>
#include <tqueue.h>

struct NodeConnection;
class Model;
class Node;

typedef boost::shared_ptr<tqueue<Node *> > sh_node_queue;

CD3_DECLARE_SIMULATION(OrderedPipeSimulation)
public:
	OrderedPipeSimulation();
	~OrderedPipeSimulation();
	void start(ptime time);
	int run(ptime time, int dt);
	NodeConnection *createConnection(Node *, const std::string &,
									 Node *, const std::string &) const;
private:
	std::vector<Node *> getOrder();
	std::vector<Node *> order;
};

#endif // ORDEREDPIPESIMULATION_H
