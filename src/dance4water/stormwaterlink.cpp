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


