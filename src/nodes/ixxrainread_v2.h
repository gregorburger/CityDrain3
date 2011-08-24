#ifndef IXXRAINREAD_V2_H
#define IXXRAINREAD_V2_H

#include <node.h>
#include <flow.h>

struct IxxRainRead_v2_Private;

CD3_DECLARE_NODE(IxxRainRead_v2)
public:
	IxxRainRead_v2();
	virtual ~IxxRainRead_v2();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);

	void start();
	void stop();

private:
	Flow out;
	std::string rain_file;
	IxxRainRead_v2_Private *data;
};

#endif // IXXRAINREAD_V2_H
