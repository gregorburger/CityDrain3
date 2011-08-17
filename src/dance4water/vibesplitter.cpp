#include "vibesplitter.h"
#include <flow.h>
#include <flowfuns.h>

CD3_DECLARE_NODE_NAME(VIBeSplitter)

VIBeSplitter::VIBeSplitter() {
    /*for (int i = 0; i < 2000; i++) {
        out[i] = new Flow();
    }*/

    addInPort(ADD_PARAMETERS(in));
    for (int i = 0; i < 2000; i++) {
        std::stringstream ss;
        ss << "out" << i;
        addOutPort(ss.str(), &out[i]);
    }

}

VIBeSplitter::~VIBeSplitter() {

}


int VIBeSplitter::f(ptime time, int dt) {
    (void) time;
    std::cout << "Run splitter" << std::endl;
    for (int i = 0; i < 2000; i++) {
        out[i] = in;
    }
    return dt;
}
