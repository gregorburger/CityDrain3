#ifndef BUFFEREDNODECONNECTION_H
#define BUFFEREDNODECONNECTION_H

#include <nodeconnection.h>
#include <tqueue.h>

class BufferedNodeConnection : public NodeConnection
{
public:
	BufferedNodeConnection(Node * source, const std::string &soport,
				   Node *sink, const std::string &siport);
	void push(int dt);
	void pull(int dt);

private:
	tqueue<Flow> q;
};

#endif // BUFFEREDNODECONNECTION_H
