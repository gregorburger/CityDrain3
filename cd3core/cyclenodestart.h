#ifndef CYCLENODESTART_H
#define CYCLENODESTART_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(CycleNodeStart)
public:
	CycleNodeStart();
	virtual ~CycleNodeStart();
	int f(int time, int dt);
	Flow out;
};

#endif // CYCLENODESTART_H
