#ifndef RAINREAD_H
#define RAINREAD_H

#include <node.h>
#include <flow.h>
#include <boost/tuple/tuple.hpp>

using namespace boost::tuples;

class RainReadPriv;
class QDateTime;

CD3_DECLARE_NODE(RainRead)
public:
	RainRead();
	virtual ~RainRead();
	void init(int start, int end, int dt);
	void deinit();
	int f(int time, int dt);
private:
	tuple<QDateTime, double> parseLine();
private:
	Flow out;
	std::string file_name;
	RainReadPriv *priv;
};

#endif // RAINREAD_H
