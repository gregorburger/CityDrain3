#ifndef FLOWBUFFER_H
#define FLOWBUFFER_H

struct FBPrivate;
class Flow;

#include <queue>
#include <cd3globals.h>

typedef std::pair<Flow, int> buf_item;

class CD3_PUBLIC FlowBuffer
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
