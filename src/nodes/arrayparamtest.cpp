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

#include "arrayparamtest.h"
#include <boost/lexical_cast.hpp>

CD3_DECLARE_NODE_NAME(ArrayParamTest)

ArrayParamTest::ArrayParamTest() {
	Qe = 0.0;
	addParameter(ADD_PARAMETERS(Qe));
	addParameter(ADD_PARAMETERS(C));
	addOutPort(ADD_PARAMETERS(out));
}

ArrayParamTest::~ArrayParamTest() {
}

bool ArrayParamTest::init(ptime start, ptime stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	out[0] = Qe;
	size_t nconcs = Flow::countUnits(Flow::concentration);
	if (C.size() != nconcs) {
		Logger(Warning) << "Array Parameter C must contain" << nconcs << "elements";
		return false;
	}
	for (size_t i = 0; i < C.size(); i++) {
		out[i+1] = C[i];
	}
	return true;
}


int ArrayParamTest::f(ptime time, int dt) {
	(void) time;
	return dt;
}
