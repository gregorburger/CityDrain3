#include "pymodel.h"
#include "mapbasedmodel.h"

#include <boost/python.hpp>
using namespace boost::python;

void wrap_model() {
	class_<MapBasedModel>("Model")
			.def("addNode", &MapBasedModel::addNode)
			;
}
