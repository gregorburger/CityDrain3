#ifndef CYCLENODEEND_H
#define CYCLENODEEND_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(CycleNodeEnd)
public:
	CycleNodeEnd();
	virtual ~CycleNodeEnd();
	int f(int time, int dt);
	Node *start;
	Flow in;
};

#endif // CYCLENODEEND_H
