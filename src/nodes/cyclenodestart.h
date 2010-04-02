#ifndef CYCLENODESTART_H
#define CYCLENODESTART_H

#include "../cd3core/node.h"
#include <flow.h>

CD3_DECLARE_NODE(CycleNodeStart)
public:
	CycleNodeStart();
	virtual ~CycleNodeStart();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
	Flow out;
};

#endif // CYCLENODESTART_H
