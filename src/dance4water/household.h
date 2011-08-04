#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H

#include <node.h>
#include <flow.h>

CD3_DECLARE_NODE(Household)
public:
    Household();
    ~Household();
    bool init(ptime start, ptime end, int dt);
    int f(ptime time, int dt);

private:
        Flow const_flow;
        Flow out;
        double People;
};

#endif // HOUSEHOLD_H
