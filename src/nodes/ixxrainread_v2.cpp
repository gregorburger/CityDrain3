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

#include "ixxrainread_v2.h"
#include <QDateTime>
#include <QFile>
#include <QRegExp>
#include <QStringList>

struct IxxRainRead_v2_Private {
	QDateTime first, last;
	int file_dt;
	QFile file;
};

typedef std::pair<QDateTime, double> IxxEntry;

CD3_DECLARE_NODE_NAME(IxxRainRead_v2)

IxxRainRead_v2::IxxRainRead_v2() {
	data = new IxxRainRead_v2_Private();
	addOutPort(ADD_PARAMETERS(out));
	datestring = "dd.MM.yyyy HH:mm:ss";
	addParameter(ADD_PARAMETERS(datestring));
	addParameter(ADD_PARAMETERS(rain_file));
}

IxxRainRead_v2::~IxxRainRead_v2() {
	delete data;
}

IxxEntry parseIxxLine(QString line, QString date_time_string) {
	QStringList parts = line.split(QRegExp("\\s+"));
	QDateTime dt = QDateTime::fromString(parts[0] + " " + parts[1], date_time_string);
	double value = parts[2].toDouble();
	return IxxEntry(dt, value);
}

static QDateTime pttoqt(const boost::posix_time::ptime &pt) {
	boost::gregorian::date gd = pt.date();
	boost::posix_time::time_duration gt = pt.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	return QDateTime(qdate, qtime);
}

bool IxxRainRead_v2::init(ptime start, ptime end, int dt) {
	q_datestring = QString::fromStdString(datestring);
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

	QString line = file.readLine();
	data->first = parseIxxLine(line, this->q_datestring).first;
	line = file.readLine();
	QDateTime second = parseIxxLine(line, this->q_datestring).first;
	data->file_dt = data->first.secsTo(second);
	if (data->first.secsTo(second) != dt) {
		Logger(Warning) << "dt of rain file and simulation are not equal; dt=" << (int) data->first.secsTo(second);
		//return false;
	}

	QDateTime qdt_start = pttoqt(start);
	QDateTime qdt_end = pttoqt(end);

	int secs_to_start = qdt_start < data->first ? qdt_start.secsTo(data->first) : data->first.secsTo(qdt_start);

	if ((secs_to_start % dt) != 0) {
		Logger(Error) << "rain file does not start at a timestep of the simulation";
		return false;
	}

	//    while (file.canReadLine()) {
	//        line = file.canReadLine();
	//    }
	//    data->last = parseIxxLine(line, this->q_datestring).first;

	//    if (data->last <= qdt_start || data->first >= qdt_end) {
	//        Logger(Error) << "timerange of rain_file outside simulation time";
	//        return false;
	//    }

	file.close();
	return true;
}

void IxxRainRead_v2::start() {
	data->file.setFileName(QString::fromStdString(rain_file));
	if (!data->file.open(QIODevice::ReadOnly)) {
		Logger(Error) << "could not open rain file";
	}
	data->file.seek(0);
}

void IxxRainRead_v2::stop() {
	data->file.close();
}

int IxxRainRead_v2::f(ptime _time, int dt) {
	QDateTime time = pttoqt(_time);

	//    if (time <= data->first || time >= data->last) {
	//        out.clear();
	//        return dt;
	//    }
	//while ()
	//   cd3assert(data->file.isOpen() && data->file.isReadable(), "can not read from rain file");

	IxxEntry entry = parseIxxLine(data->file.readLine(), this->q_datestring);
	double sum =entry.second;
	while (entry.first.secsTo(time) - this->data->file_dt > 0) {
		entry = parseIxxLine(data->file.readLine(), this->q_datestring);
		sum+=entry.second;

		//cd3assert(time > entry.first && entry.first.secsTo(time) == dt, "time slipped through a dark worm hole");
	}

	out[0] =sum; //do the actual work ;-)

	return dt;
}
