#include "testnode.h"
#include <flow.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

CD3_DECLARE_NODE_NAME(TestNode)

TestNode::TestNode() {
	in_out = new Flow();
	test = new Flow();
	addOutPort("out", in_out);
	addInPort("in", in_out);
	int_value = 10;
	double_value = 3.142;
	string_value = "los gehts";
	addState(ADD_PARAMETERS(int_value));
	addState(ADD_PARAMETERS(double_value));
	addState(ADD_PARAMETERS(string_value));
	addState(ADD_PARAMETERS_P(test));
}

TestNode::~TestNode() {
	delete in_out;
}

int TestNode::f(int time, int dt) {
	int_value += dt;
	double_value += time;
	string_value += str(boost::format(" hallo %1% ") % time);

	test->dump();
	*test = Flow();
	test->dump();

	return dt;
}
