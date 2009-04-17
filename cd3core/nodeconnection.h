#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <utility>
#include <string>
#include <cd3globals.h>

class Node;

typedef std::pair<const std::string &, Node *> endpoint;

class CD3_PUBLIC NodeConnection
{
public:
	NodeConnection(const std::string &soport, Node * source,
				   const std::string &siport, Node *sink);

	const endpoint &getSink();
	const endpoint &getSoure();

private:
	endpoint source, sink;
};

#endif // NODECONNECTION_H
