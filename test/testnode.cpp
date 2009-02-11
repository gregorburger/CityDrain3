#include "testnode.h"

#include <iostream>

TestNode::TestNode() {
	addParameter(ADD_PARAMETERS(num_subreaches));
}

TestNode::~TestNode() {
}

void TestNode::f(int time, int dt) {
	(void) time;
	(void) dt;
	std::cout << "ix = " << ix << std::endl;
}

void TestNode::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	addOutPort(ADD_PARAMETERS(out));
	addInPort(ADD_PARAMETERS(in));
	addState(ADD_PARAMETERS(ix));
}
