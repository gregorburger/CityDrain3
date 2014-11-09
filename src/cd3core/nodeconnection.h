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

#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <utility>
#include <string>
#include "tqueue.h"
#include <flow.h>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>
using namespace boost;

class Node;

typedef std::pair<std::string, Node *> endpoint;

struct CD3_PUBLIC NodeConnection
{
	NodeConnection(Node * source, const std::string &soport,
				   Node *sink, const std::string &siport);

	virtual ~NodeConnection();

	Node *source, *sink;
	std::string source_port, sink_port;

	virtual void pull(int dt);
	virtual void push(int dt);
};

#endif // NODECONNECTION_H
