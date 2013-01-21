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

#include "flowreadsimple.h"
#include <QDateTime>
#include <QFile>
#include <QRegExp>
#include <QStringList>
//#include <iostream>

struct FlowReadSimple_Private {
	QDateTime first, last;
	QFile file;
};

typedef std::pair<QDateTime, std::vector<double> > IxxEntry;

CD3_DECLARE_NODE_NAME(FlowReadSimple)

FlowReadSimple::FlowReadSimple() {
	data = new FlowReadSimple_Private();
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(rain_file));
}

FlowReadSimple::~FlowReadSimple() {
	delete data;
}

static
IxxEntry parseIxxLine(QString line) {
	QStringList parts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	QDateTime dt = QDateTime::fromString(parts[0] + " " + parts[1], "dd.MM.yyyy HH:mm:ss");
	std::vector<double> value;
	for (int i = 2; i < parts.size(); i++) {
		value.push_back(parts[i].toDouble());
	}

	return IxxEntry(dt, value);
}

static QDateTime pttoqt(const boost::posix_time::ptime &pt) {
	boost::gregorian::date gd = pt.date();
	boost::posix_time::time_duration gt = pt.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	return QDateTime(qdate, qtime);
}

bool FlowReadSimple::init(ptime start, ptime end, int dt) {
	// Foollowing are test routines for the file
	// Test if rainfile exists and can be opened
	QString q_rain_file = QString::fromStdString(rain_file);
	if (!QFile::exists(q_rain_file)) {
		Logger(Error) << "rain_file does not exist";
		return false;
	}
	QFile file(q_rain_file);
	if (!file.open(QIODevice::ReadOnly)) {
		Logger(Error) << "could not open rain_file";
		return false;
	}

	// Test if rainfile has correct dt spacing == only tested for the first 2 lines
	QString line = file.readLine();
	data->first = parseIxxLine(line).first;
	line = file.readLine();
	QDateTime second = parseIxxLine(line).first;

	if (data->first.secsTo(second) != dt) {
		Logger(Error) << "dt of rain file and simulation must be equal";
		return false;
	}

	QDateTime qdt_start = pttoqt(start);
	QDateTime qdt_end = pttoqt(end);

	int secs_to_start = qdt_start < data->first ? qdt_start.secsTo(data->first) : data->first.secsTo(qdt_start);

	if ((secs_to_start % dt) != 0) {
		Logger(Error) << "rain file does not start at a timestep of the simulation";
		return false;
	}

	// Go to last line and read last date >> check
	while(!(file.atEnd())) {
		line = file.readLine();
	}
	data->last = parseIxxLine(line).first;

	if (data->last <= qdt_start || data->first >= qdt_end) {
		Logger(Error) << "timerange of rain_file outside simulation time";
		return false;
	}

	file.close();
	return true;
}

void FlowReadSimple::start() {
	data->file.setFileName(QString::fromStdString(rain_file));
	if (!data->file.open(QIODevice::ReadOnly)) {
		Logger(Error) << "could not open rain file";
	}
	data->file.seek(0);
}

void FlowReadSimple::stop() {
	data->file.close();
}

int FlowReadSimple::f(ptime _time, int dt) {
	QDateTime time = pttoqt(_time);

	if (time <= data->first || time >= data->last) {
		out.clear();
		return dt;
	}

	cd3assert(data->file.isOpen() && data->file.isReadable(), "can not read from rain file");

	IxxEntry entry = parseIxxLine(data->file.readLine());


	//eat up all entry in file before simulation time
	while (entry.first < time) {
		entry = parseIxxLine(data->file.readLine());
	}

	for (int i = 0; i < std::min(entry.second.size(), out.size()); i++) {
		out[i] = entry.second[i]; //do the actual work ;-)
	}

	return dt;
}
