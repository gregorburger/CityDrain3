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

void test_flow(Flow *f) {
	assert(f);
	BOOST_FOREACH(string n, f->getNames()) {
		cout << "test_flow " << n << endl;
	}
}

void wrap_flow() {
	class_<Flow>("Flow")
			.def("define", &Flow::define)
			.staticmethod("define")
			;
	enum_<Flow::CalculationUnit>("CalculationUnit")
			.value("flow", Flow::flow)
			.value("concentration", Flow::concentration)
			;

	class_<fd>("FlowDefinition")
			.def("__len__", &fd::size)
			.def("__setitem__", &map_item::set)
			;
	def("test_flow", test_flow);
}
