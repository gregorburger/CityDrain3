#ifndef CRAINBUFFER_H
#define CRAINBUFFER_H

#include <utility>
#include <deque>
#include <vector>

typedef std::pair<int, std::vector<double> > ixx_q_event;

class IxxQBuffer
{
public:
	IxxQBuffer();

	void put(int dt, std::vector<double> values);
	std::vector<double> take(int dt);
	int available() const;
	void clear();
private:
	int dt;
	std::deque<ixx_q_event> buffer;
	std::deque<int>	avail;
};

#endif // CRAINBUFFER_H
