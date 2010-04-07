#include "pymodel.h"
#include "mapbasedmodel.h"
#include "nodeconnection.h"
#include "node.h"

#include <boost/python.hpp>
using namespace boost::python;

/** transfer ownership*/
static void m_addNode(MapBasedModel &model, const string &id, auto_ptr<Node> node) {
	model.addNode(id, node.get());
	node.release();
}

/** transfer ownership*/
static void m_addConnection(MapBasedModel &model, auto_ptr<NodeConnection> con) {
	model.addConnection(con.get());
	con.release();
}

void wrap_model() {
	class_<IModel, boost::noncopyable>("IModel", no_init)
		;
	class_<MapBasedModel, bases<IModel> >("Model")
		.def("addNode", m_addNode)
		.def("getNode", &MapBasedModel::getNode, return_internal_reference<>())
		.def("addConnection", m_addConnection)
		;
	implicitly_convertible<auto_ptr<MapBasedModel>, auto_ptr<IModel> >();
}
