#include "vibesplitter.h"
#include <flow.h>
#include <flowfuns.h>

CD3_DECLARE_NODE_NAME(VIBeSplitter)

VIBeSplitter::VIBeSplitter() {
    in = new Flow();
    for (int i = 0; i < 2000; i++) {
        out[i] = new Flow();
    }

    addInPort(ADD_PARAMETERS_P(in));
    for (int i = 0; i < 2000; i++) {
        std::stringstream ss;
        ss << "out" << i;
        addOutPort(ss.str(), out[i]);
    }

}

VIBeSplitter::~VIBeSplitter() {
    for (int i = 0; i < 2000; i++) {
        delete out[i];
    }
    delete in;
}

bool VIBeSplitter::init(ptime start, ptime end, int dt) {
    return true;
}

int VIBeSplitter::f(ptime time, int dt) {
    (void) time;

    for (int i = 0; i < 2000; i++) {
        *out[i] = *(in);
    }
    return dt;
}
