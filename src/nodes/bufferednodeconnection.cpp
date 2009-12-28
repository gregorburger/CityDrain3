#include "bufferednodeconnection.h"
#include <node.h>

BufferedNodeConnection::BufferedNodeConnection(Node * source, const std::string &soport,
                                   Node *sink, const std::string &siport)
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
