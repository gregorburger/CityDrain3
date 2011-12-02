#include "mcso.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <flowfuns.h>

using namespace boost;

#include <cd3assert.h>

CD3_DECLARE_NODE_NAME(mCSO)

mCSO::mCSO() {
	addInPort(ADD_PARAMETERS(in));
	addOutPort(ADD_PARAMETERS(out));
	addOutPort(ADD_PARAMETERS(overflow));

	V_Max = 300;
	Q_Max = 0.1;

	efficency = std::vector<double>(Flow::countUnits(Flow::concentration),1.5);


	addParameter(ADD_PARAMETERS(V_Max))
		.setUnit("m3");
	addParameter(ADD_PARAMETERS(Q_Max))
		.setUnit("m3/s");
	addParameter(ADD_PARAMETERS(efficency))
		.setUnit("-");

	addState(ADD_PARAMETERS(stored_volume));
}

mCSO::~mCSO() {
}

bool mCSO::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;
	stored_volume.clear();
	return true;
}

int mCSO::f(ptime time, int dt) {
	(void) time;
	out.clear();
	overflow.clear();

	std::vector<Flow*> flows;
	flows.push_back(&stored_volume);
	flows.push_back(&in);
	stored_volume = FlowFuns::mix(flows);



	if(stored_volume[0]<=(Q_Max*dt)){
		out[0]=stored_volume[0];
		stored_volume[0]=0;
		for (size_t i = 1; i < Flow::size(); i++) {
			out[i] = stored_volume[i];
		}
	}
	else{
		out[0]=(Q_Max*dt);
		stored_volume[0]-=out[0];
		if(stored_volume[0]>V_Max){
			overflow[0] = stored_volume[0]-V_Max;
			stored_volume[0]-=overflow[0];

			for (size_t i = 1; i < Flow::size(); i++) {
				out[i] = stored_volume[i]*efficency[i-1];
				overflow[i] = ((out[0]+overflow[0])*stored_volume[i]-out[i]*out[0])/overflow[0];
				if(overflow[i]<0){
					out[i]=(stored_volume[i]*(out[0]+overflow[0]))/out[i];
					overflow[i]=0;
				}
			}
		}
		else{
			for (size_t i = 1; i < Flow::size(); i++) {
				out[i] = stored_volume[i];
			}
		}
	}
	return dt;
}
