#include "vibemixer.h"

CD3_DECLARE_NODE_NAME(VIBeMixer)

VIBeMixer::VIBeMixer() {
    //Q = new double;
    //*Q = 0.0;
    addInPort(ADD_PARAMETERS(in_1));
    addInPort(ADD_PARAMETERS(in_2));
    addInPort(ADD_PARAMETERS(in_3));
    addInPort(ADD_PARAMETERS(in_4));
    addInPort(ADD_PARAMETERS(in_5));
    addInPort(ADD_PARAMETERS(in_6));
    addOutPort(ADD_PARAMETERS(out));
    //addState("Q", Q);
}


int VIBeMixer::f(ptime time, int dt) {

    out[0] = in_1[0] + in_2[0] + in_3[0] + in_4[0] + in_5[0]+ in_6[0];
    //*Q = out[0];
    for (int i = 1; i < Flow::size(); i++) {
        out[i] = (in_1[1]*in_1[0] + in_2[i]*in_2[0]) / out[0];

    }
    return dt;

}
