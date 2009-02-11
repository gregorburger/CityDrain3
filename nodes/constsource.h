#ifndef CONSTSOURCE_H
#define CONSTSOURCE_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(ConstSource)
public:
	ConstSource();
	~ConstSource();
	void f(int time, int dt);
	void init(int start, int end, int dt);
private:
	Flow const_flow;
	Flow out;
	Flow *tmp;
};

#endif // CONSTSOURCE_H
