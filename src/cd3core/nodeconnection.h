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

	Node *source, *sink;
	std::string source_port, sink_port;

	virtual void pull(int dt);
	virtual void push(int dt);
};

#endif // NODECONNECTION_H
