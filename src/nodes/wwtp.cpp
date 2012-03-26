/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#include "wwtp.h"
#include "rumba01dll.h"

CD3_DECLARE_NODE_NAME(WWTP)

WWTP::WWTP() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	temperature = 15.0;
	addParameter(ADD_PARAMETERS(temperature));
	addParameter(ADD_PARAMETERS(logfile));
	sysdata = "../data/rumba/sys_data.txt";
	addParameter(ADD_PARAMETERS(sysdata));
	parameter = "../data/rumba/wtp_para.txt";
	addParameter(ADD_PARAMETERS(parameter));
	odedata = "../data/rumba/ode_para.txt";
	addParameter(ADD_PARAMETERS(odedata));
}

WWTP::~WWTP() {

}

bool WWTP::init(ptime start, ptime end, int dt) {
	(void) start;
	(void) end;
	(void) dt;
	float CSe,CPe,Ne,Pe,SBLHe;
	if (!init_wtp(logfile.c_str(),
				  sysdata.c_str(),
				  parameter.c_str(),
				  odedata.c_str())) {
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
