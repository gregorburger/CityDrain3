#ifndef RAINBUFFER_H
#define RAINBUFFER_H

#include <utility>
#include <deque>

typedef std::pair<int, double> rain_event;

class RainBuffer
{
public:
	RainBuffer();

	void put(int dt, double value);
	double take(int dt);
	int available() const;
	void clear();
private:
	int dt;
	std::deque<rain_event> buffer;
	std::deque<int>	avail;
};

#endif // RAINBUFFER_H
