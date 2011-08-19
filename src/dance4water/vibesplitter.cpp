#include "vibesplitter.h"
#include <flow.h>
#include <flowfuns.h>

CD3_DECLARE_NODE_NAME(VIBeSplitter)

VIBeSplitter::VIBeSplitter() {
    outports = 300;
    for (int i = 0; i < outports; i++) {
        out.push_back( new Flow);
    }
    in = new Flow;
    addInPort(ADD_PARAMETERS_P(in));
    for (int i = 0; i < outports; i++) {
        std::stringstream ss;
        ss << "out" << i;
        addOutPort(ss.str(), out[i]);
    }

}

VIBeSplitter::~VIBeSplitter() {
for (int i = 0; i < outports; i++) {
    delete out[i];
}
}


int VIBeSplitter::f(ptime time, int dt) {
    (void) time;
    for (int i = 0; i < outports; i++) {
        *(out[i]) = *in;
    }
    return dt;
}
