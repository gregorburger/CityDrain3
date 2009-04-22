#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <utility>
#include <string>
#include "tqueue.h"
#include <flow.h>
#include <cd3globals.h>

class Node;

typedef std::pair<std::string, Node *> endpoint;

struct CD3_PUBLIC NodeConnection
{
	NodeConnection(Node * source, const std::string &soport,
				   Node *sink, const std::string &siport);

	Node *source, *sink;
	std::string source_port, sink_port;

	void pull();
	void push();

private:
	tqueue<Flow> q;
};

#endif // NODECONNECTION_H
