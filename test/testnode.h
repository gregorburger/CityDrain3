#ifndef TESTNODE_H
#define TESTNODE_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(TestNode)
public:
    TestNode();
	~TestNode();
	void f(int time, int dt);

	void init(int start, int stop, int dt);

private:
	int ix;
	double dx;
	std::string sx;
	int num_subreaches;

	Flow in, out;
};

#endif // TESTNODE_H
