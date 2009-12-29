#include "pymodel.h"
#include "mapbasedmodel.h"
#include "nodeconnection.h"
#include "node.h"

#include <boost/python.hpp>
using namespace boost::python;

/** transfer ownership*/
static void m_addNode(MapBasedModel &model, auto_ptr<Node> node) {
	model.addNode(node.get());
	node.release();
}

/** transfer ownership*/
static void m_addConnection(MapBasedModel &model, auto_ptr<NodeConnection> con) {
	model.addConnection(con.get());
	con.release();
}

void wrap_model() {
	class_<MapBasedModel>("Model")
		.def("addNode", m_addNode)
		.def("addConnection", m_addConnection)
		;
}
