#ifndef IXXRAINREAD_H
#define IXXRAINREAD_H

#include <node.h>
#include <string>

#include "flow.h"

struct IxxRainReadPrivate;

CD3_DECLARE_NODE(IxxRainRead)
public:
	IxxRainRead();
	virtual ~IxxRainRead();
	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
private:
	IxxRainReadPrivate *data;
	Flow out;
	std::string rain_file;
};

#endif // IXXRAINREAD_H
