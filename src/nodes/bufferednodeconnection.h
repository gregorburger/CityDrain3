#ifndef BUFFEREDNODECONNECTION_H
#define BUFFEREDNODECONNECTION_H

#include <nodeconnection.h>
#include <tqueue.h>

class BufferedNodeConnection : public NodeConnection
{
public:
	BufferedNodeConnection(shared_ptr<Node>  source, const std::string &soport,
				   shared_ptr<Node> sink, const std::string &siport);
	virtual ~BufferedNodeConnection(){}
	void push(int dt);
	void pull(int dt);

private:
	tqueue<Flow> q;
};

#endif // BUFFEREDNODECONNECTION_H
