#ifndef IXXQREAD_H
#define IXXQREAD_H

#include <node.h>
#include <string>

#include "flow.h"

struct IxxQReadPrivate;

CD3_DECLARE_NODE(IxxQRead)
public:
        IxxQRead();
        virtual ~IxxQRead();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
        IxxQReadPrivate *data;
	Flow out;
	std::string in_file;
};

#endif // IXXQREAD_H
