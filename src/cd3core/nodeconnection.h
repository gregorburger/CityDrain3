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

typedef std::pair<std::string, shared_ptr<Node> > endpoint;

struct CD3_PUBLIC NodeConnection
{
	NodeConnection(shared_ptr<Node>  source, const std::string &soport,
				   shared_ptr<Node> sink, const std::string &siport);

	shared_ptr<Node> source, sink;
	std::string source_port, sink_port;

	virtual void pull(int dt);
	virtual void push(int dt);
};

#endif // NODECONNECTION_H
