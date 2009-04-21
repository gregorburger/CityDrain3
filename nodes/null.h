#ifndef NULL_H
#define NULL_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Null)
public:
	Null();
	int f(int time, int dt);
private:
	Flow in_out;
};

#endif // NULL_H
