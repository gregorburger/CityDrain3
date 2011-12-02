#ifndef FLOWREAD_H
#define FLOWREAD_H



#include "flow.h"

#include "flowreadcontext.h"

CD3_DECLARE_NODE(FlowRead)
public:
	FlowRead();
	virtual ~FlowRead();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);
	void deinit();

	void start();
	void stop();

private:
	Flow out;
	std::string filename;
	FlowReadContext ctxt;
};


#endif // FLOWREAD_H
