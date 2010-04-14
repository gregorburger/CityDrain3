#include "pyflow.h"
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <string>
#include <iostream>
using namespace boost::python;
using namespace std;

#include <flow.h>

double flow_getitem(Flow &f, int i) {
	return f[i];
}

void flow_setitem(Flow &f, int i, double v) {
	f[i] = v;
}

void test_flow(Flow *f) {
	assert(f);
	BOOST_FOREACH(string n, f->getNames()) {
		cout << "test_flow " << n << endl;
	}
}

void wrap_flow() {
	class_<Flow, boost::noncopyable>("Flow")
			.def("define", &Flow::define, "call first to globally define the flow")
			.def("__len__", &Flow::size, "return the number of entries in each flow")
			.def("__getitem__", flow_getitem, "index based value lookup")
			.def("__setitem__", flow_setitem, "index based value update")
			.def("getNames", &Flow::getNames, "return all defined names")
			.def("setValue", &Flow::setValue, "set value by name")
			.def("getValue", &Flow::getValue, "get value by name")
			.def("clear", &Flow::clear, "clear all values")
			.def("getIth", &Flow::getIth, "get ith value by unit")
			.def("setIth", &Flow::setIth, "set ith value by unit")
			.def("countUnits", &Flow::countUnits, "gets number of defined values per unit")
			.def("dump", &Flow::dump, "dump to stdout")
			.def("getUnit", &Flow::getUnit, "get unit by name")
			.staticmethod("define")
			.staticmethod("__len__")
			.staticmethod("getNames")
			.staticmethod("getUnit")
			;
	enum_<Flow::CalculationUnit>("CalculationUnit")
			.value("flow", Flow::flow)
			.value("concentration", Flow::concentration)
			;

	class_<std::map<string, Flow::CalculationUnit> >("FlowDefinition")
			.def(map_indexing_suite<std::map<string, Flow::CalculationUnit> >())
			;
}
