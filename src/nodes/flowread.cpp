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

#include "flowread.h"


CD3_DECLARE_NODE_NAME(FlowRead)

FlowRead::FlowRead() {
	ctxt.dateformat = "dd.MM.yyyy hh:mm:ss";
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(filename));
	addParameter(ADD_PARAMETERS(ctxt.dateformat));
}

FlowRead::~FlowRead() {
}

bool FlowRead::init(ptime start, ptime end, int dt) {
	return true;
}

void FlowRead::deinit() {

}

int FlowRead::f(ptime time, int dt) {

	boost::gregorian::date gd = time.date();
	boost::posix_time::time_duration gt = time.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	QDateTime current(qdate, qtime);

	ctxt.setOutPort(&out, current, dt);
	return dt;
}

void FlowRead::start() {
	ctxt.file.setFileName(QString::fromStdString(filename));
	if (!ctxt.file.open(QIODevice::ReadOnly)) {
		Logger(Error) << "could not open file";
		return;
	}
	while(!ctxt.buffer.empty()){
		delete ctxt.buffer.front();
		ctxt.buffer.pop();
	}
	ctxt.availabe=0;
}

void FlowRead::stop() {
	ctxt.file.close();

}



