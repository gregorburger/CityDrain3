#include "wwtp.h"
#include "rumba01dll.h"

CD3_DECLARE_NODE_NAME(WWTP)

WWTP::WWTP() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	temperature = 15.0;
	addParameter(ADD_PARAMETERS(temperature));
}

WWTP::~WWTP() {

}

bool WWTP::init(ptime start, ptime end, int dt) {
	float CSe,CPe,Ne,Pe,SBLHe;
	if (!init_wtp()) {
		return false;
	}
	init_relaxation(1.0, 15.0, 2.0,
					200.0, 400.0, 50.0, 10.0,
					&CSe, &CPe, &Ne, &Pe, &SBLHe, .001);
	return true;
}

void WWTP::deinit() {
	exit_wtp();
}

int WWTP::f(ptime time, int dt) {
	float CODs,
		CODp,
		Ntot,
		Ptot,
		CODs_eff,
		CODp_eff,
		Ntot_eff,
		Ptot_eff,
		sblh_eff,
		temp;
	temp = temperature;

	CODs = in.getValue("CODs");
	CODp = in.getValue("CODp");
	Ntot = in.getValue("Ntot");
	Ptot = in.getValue("Ptot");

	run_timestep(dt / 86400.0, temp, in[0], CODs, CODp, Ntot, Ptot, &CODs_eff, &CODp_eff, &Ntot_eff, &Ptot_eff, &sblh_eff);

	out[0] = in[0];
	out.setValue("CODs", CODs_eff);
	out.setValue("CODp", CODp_eff);
	out.setValue("Ntot", Ntot_eff);
	out.setValue("Ptot", Ptot_eff);

	return dt;
}
