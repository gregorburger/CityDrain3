#include "impuls.h"

CD3_DECLARE_NODE_NAME(Impuls)

Impuls::Impuls() {
	start_after = 0;
	length = 1200;
	addParameter(ADD_PARAMETERS(start_after))
		.setUnit("s");
	addParameter(ADD_PARAMETERS(length))
		.setUnit("s");
	addParameter(ADD_PARAMETERS(Q))
		.setUnit("m^3/s");
	addOutPort(ADD_PARAMETERS(out));
}

Impuls::~Impuls() {
}

bool Impuls::init(ptime start, ptime end, int dt) {
	if (start_after % dt) {
		Logger(Warning) << "start_after must be a multiple of dt";
		return false;
	}

	if (length % dt) {
		Logger(Warning) << "length must be a multiple of dt";
		return false;
	}

	this->start = start + seconds(start_after);
	this->stop = this->start + seconds(length);
	q.clear();
	return true;
}

int Impuls::f(ptime time, int dt) {
	if (time >= this->start && time < this->stop) {
		q[0] = Q * dt;
		out = q;
	} else {
		out = null;
	}
	return dt;
}
