#ifndef STORMWATERLINK_H
#define STORMWATERLINK_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(StormwaterLink)


public:
    StormwaterLink();
    ~StormwaterLink();


    int f(ptime time, int dt);
private:
    Flow out;
    Flow in;
};

#endif // STORMWATERLINK_H
