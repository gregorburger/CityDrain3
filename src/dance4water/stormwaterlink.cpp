#include "stormwaterlink.h"
#include <flow.h>

CD3_DECLARE_NODE_NAME(StormwaterLink)

StormwaterLink::StormwaterLink() {

    addOutPort(ADD_PARAMETERS(out));
    addInPort(ADD_PARAMETERS(in));

}

StormwaterLink::~StormwaterLink() {
}

int StormwaterLink::f(ptime time, int dt) {
    (void) time;
    out[0] = in[0];
    return dt;
}

bool StormwaterLink::init(ptime start, ptime end, int dt) {

    (void) start;
    (void) end;
    out[0] = 0;
    return true;
}

