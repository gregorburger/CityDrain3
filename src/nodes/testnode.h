#ifndef TESTNODE_H
#define TESTNODE_H

#include <node.h>

class Flow;

CD3_DECLARE_NODE(TestNode)
public:
	TestNode();
	virtual ~TestNode();
	int f(int time, int dt);

private:
	Flow *in_out;
	Flow *test;
	int int_value;
	double double_value;
	std::string string_value;
};

#endif // TESTNODE_H
