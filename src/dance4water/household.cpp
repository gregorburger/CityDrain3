#include "household.h"

#include <flow.h>

CD3_DECLARE_NODE_NAME(Household)

Household::Household() {
    addParameter(ADD_PARAMETERS(People));
        addParameter(ADD_PARAMETERS(const_flow))
                .setUnit("m^3/s, g/m^3");
        addOutPort(ADD_PARAMETERS(out));
}

Household::~Household() {
}

int Household::f(ptime time, int dt) {
        (void) time;
        return dt;
}

bool Household::init(ptime start, ptime end, int dt) {
        (void) start;
        (void) end;
        //out = const_flow;
        out[0] = const_flow[0] * dt * People;
        return true;
}

