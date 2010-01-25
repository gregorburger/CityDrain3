#ifndef CONSTSOURCE_H
#define CONSTSOURCE_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(ConstSource)
public:
	ConstSource();
	~ConstSource();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	Flow const_flow;
	Flow out;
};

#endif // CONSTSOURCE_H
