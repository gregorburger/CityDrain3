#include "simplepump.h"
#include <QDebug>

CD3_DECLARE_NODE_NAME(SimplePump)


SimplePump::SimplePump() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out_w));
	addOutPort(ADD_PARAMETERS(out_p));
	Qp = 0.005;
	Voff = 0;
	Von = 1.5;
	Vmax = 1.5;
	addParameter(ADD_PARAMETERS(Von))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Voff))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Qp))
		.setUnit("m^3/s");
	addParameter(ADD_PARAMETERS(Vmax))
		.setUnit("m^3");
}

SimplePump::~SimplePump() {

}

int SimplePump::f(ptime time, int dt) {
	(void) time;
	volume[0] += in[0] * dt;
	if (volume[0] > Von) {
		double volume_after_pump = max(Voff, volume[0] - Qpp);
		out_p[0] = (volume[0] - volume_after_pump)/dt;
		volume[0] = volume_after_pump;
		out_w[0] = max(0.0, volume[0] - Vmax)/dt;
		volume[0] -= out_w[0]*dt;
	} else {
		out_p[0] = 0.0;
		out_w[0] = 0.0;
	}
	return dt;
}

bool SimplePump::init(ptime start, ptime end, int dt) {
	if (Voff >= Von) {
		Logger(Warning) << "Von must be smaller than Voff";
		return false;
	}

	if (Von > Vmax) {
		Logger(Warning) << "Von must be smaller than Vmax";
		return false;
	}
	volume.clear();
	Qpp = Qp * dt;
	return true;
}

void SimplePump::deinit() {

}
