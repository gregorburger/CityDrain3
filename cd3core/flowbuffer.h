#ifndef FLOWBUFFER_H
#define FLOWBUFFER_H

struct FBPrivate;
class Flow;

class FlowBuffer
{
public:
	FlowBuffer();
	virtual ~FlowBuffer();
	void put(const Flow *f, int dt);
	Flow *take(int dt);
	int buffered() const;
private:
	Flow *	buffer;
	int		buffereddt;
};

#endif // FLOWBUFFER_H
