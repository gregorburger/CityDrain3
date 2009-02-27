#ifndef FLOWBUFFER_H
#define FLOWBUFFER_H

struct FBPrivate;
class Flow;

#include <queue>

typedef std::pair<Flow, int> buf_item;

class FlowBuffer
{
public:
	FlowBuffer();
	virtual ~FlowBuffer();
	void put(const Flow f, int dt);
	Flow take(int dt);
	int buffered() const;
private:
	std::queue<buf_item> buffer;
	int	buffereddt;
};

#endif // FLOWBUFFER_H
