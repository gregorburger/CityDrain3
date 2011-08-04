#include "storage.h"
#include <flow.h>

CD3_DECLARE_NODE_NAME(Storage)

Storage::Storage() {
    this->state = new double;

    addOutPort(ADD_PARAMETERS(out));
    addInPort(ADD_PARAMETERS(in));
    addState("StorageVolume", state);

}

Storage::~Storage() {
}

int Storage::f(ptime time, int dt) {
    (void) time;
    StorageVolume += in[0];
    out[0] = StorageVolume;
    *state = StorageVolume;


    return dt;
}

bool Storage::init(ptime start, ptime end, int dt) {
    StorageVolume = 0;
    (void) start;
    (void) end;
    out[0] = 0;
    return true;
}

