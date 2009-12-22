#include "pyflow.h"
#include <boost/python.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <iostream>
using namespace boost::python;
using namespace std;


#include <flow.h>

typedef map<string, Flow::CalculationUnit> fd;

struct map_item
{
	typedef string K;
	typedef Flow::CalculationUnit V;
	static V& get(fd & x, K const& i)
	{
		if( x.find(i) != x.end() ) return x[i];
		//KeyError();
	}
	static void set(fd & x, K const& i, V const& v)
	{
		x[i]=v; // use map autocreation feature
	}
	static void del(fd & x, K const& i)
	{
		if( x.find(i) != x.end() ) x.erase(i);
		//else KeyError();
	}
};



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

list flow_getnames() {
	list s;
	BOOST_FOREACH(string name, Flow::getNames()) {
		s.append(name);
	}
	return s;
}

void wrap_flow() {
	class_<Flow>("Flow")
			.def("define", &Flow::define, "call first to globally define the flow")
			.def("__len__", &Flow::size, "return the number of entries in each flow")
			.def("__getitem__", flow_getitem, "index based value lookup")
			.def("__setitem__", flow_setitem, "index based value update")
			.def("names", &flow_getnames, "return all defined names")
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
			.staticmethod("names")
			.staticmethod("getUnit")
			;
	enum_<Flow::CalculationUnit>("CalculationUnit")
			.value("flow", Flow::flow)
			.value("concentration", Flow::concentration)
			;

	class_<fd>("FlowDefinition", "Use like a dictionary with string keys"
			   ", and CalculationUnit values.\ne.g.\n"
			   "fd = pycd3.FlowDefinition()\n"
			   "fd['Q'] = pycd3.CalculationUnit.flow\n"
			   "pycd3.Flow.define(fd)")
			.def("__len__", &fd::size)
			.def("__setitem__", &map_item::set)
			;
	//def("test_flow", test_flow, "call to test a flow instance");
}
