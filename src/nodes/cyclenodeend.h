#ifndef CYCLENODEEND_H
#define CYCLENODEEND_H

#include "../cd3core/node.h"
#include <flow.h>

class CycleNodeStart;

CD3_DECLARE_NODE(CycleNodeEnd)
public:
	CycleNodeEnd();
	virtual ~CycleNodeEnd();
	int f(int time, int dt);
	CycleNodeStart *start;
	Flow in;
};

#endif // CYCLENODEEND_H
