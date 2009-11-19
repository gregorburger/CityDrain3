#include "bufferednodeconnection.h"
#include <node.h>

BufferedNodeConnection::BufferedNodeConnection(shared_ptr<Node>  source, const std::string &soport,
				   shared_ptr<Node> sink, const std::string &siport)
 : NodeConnection(source, soport, sink, siport) {

}

void BufferedNodeConnection::pull(int dt) {
	(void) dt;
	Flow f = q.dequeue();
	sink->setInPort(sink_port, &f);
}

void BufferedNodeConnection::push(int dt) {
	(void) dt;
	q.enqueue(*source->getOutPort(source_port));
}
