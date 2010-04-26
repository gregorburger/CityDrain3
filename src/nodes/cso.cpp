#include "cso.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace boost;

#include <cd3assert.h>

CD3_DECLARE_NODE_NAME(CSO)

CSO::CSO() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	addOutPort(ADD_PARAMETERS(overflow));

	V_Max = 300;
	Q_Max = 0.1;

	addParameter(ADD_PARAMETERS(V_Max))
		.setUnit("m3");
	addParameter(ADD_PARAMETERS(Q_Max))
		.setUnit("m3/s");

	addState(ADD_PARAMETERS(stored_volume));
	stored_volume.clear();
}

CSO::~CSO() {
}

bool CSO::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;
	return true;
}

int CSO::f(ptime time, int dt) {
	(void) time;
	Flow V_old = stored_volume;
	//Vii=(u(1)-Qemax)*tstep+x(1);
	double Vii = in[0] - Q_Max*dt + V_old[0];

	if (Vii < 0) { //case 1
		//	Vi=0;
		stored_volume[0] = 0;
		//	Qei=x(1)/tstep+u(1);
		out[0] = V_old[0] + in[0];
		//	Qwi=0;
		overflow[0] = 0;
	}
	if (Vii > V_Max) {//case 2
		//Vi=Vmax;
		stored_volume[0] = V_Max;
		//Qei=Qemax;
		out[0] = Q_Max*dt;
		//Qwi=u(1)-Qemax-(Vmax-x(1))/tstep;
		overflow[0] = in[0] - Q_Max*dt - (V_Max - V_old[0]);
	}
	if (Vii >= 0 && Vii <= V_Max) {//case 3
		//Vi=(u(1)-Qemax)*tstep+x(1);
		stored_volume[0] = in[0] - Q_Max * dt + V_old[0];
		//Qei=Qemax;
		out[0] = Q_Max * dt;
		//Qwi=0;
		overflow[0] = 0;
	}

	double VPrime = in[0] * dt + V_old[0];

	if (VPrime > 0) {
		for (size_t i = 1; i < Flow::size(); i++) {
			double c = (in[i]*in[0] * dt + V_old[i] * V_old[0]) / VPrime;
			out[i] = overflow[i] = stored_volume[i] = c;
		}
	}

	return dt;
}
