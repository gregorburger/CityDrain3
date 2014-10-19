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

#include "fileout.h"

#include <flow.h>
#include <logger.h>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <boost/foreach.hpp>

CD3_DECLARE_NODE_NAME(FileOut)

FileOut::FileOut()
	: Node(), stream(&file) {
	out_file_name = "out.txt";
	addParameter(ADD_PARAMETERS(out_file_name));
	addInPort(ADD_PARAMETERS(in));
	addOutPort("out", &in);
}

FileOut::~FileOut() {
}

bool FileOut::init(ptime start, ptime stop, int dt) {
	(void) dt;
	if (out_file_name == "") {
		Logger(Warning) << this << "out_file_name not defined";
		return false;
	}
	QString q_out_file_name = QString::fromStdString(out_file_name);
	if (QFile::exists(q_out_file_name)) {
		Logger(Warning) << this << "overwriting file " << out_file_name;
	}
	QFileInfo info(q_out_file_name);
	if (!info.absoluteDir().exists()) {
		Logger(Warning) << this << "parent dir of" << out_file_name << "does not exist";
		return false;
	}
	file.setFileName(q_out_file_name);
	Logger() << this << "using fileout:" << out_file_name;
	stream.setRealNumberPrecision(10);
	return true;
}

void FileOut::deinit() {
}

QDateTime pttoqt(const boost::posix_time::ptime &pt) {
	boost::gregorian::date gd = pt.date();
	boost::posix_time::time_duration gt = pt.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	return QDateTime(qdate, qtime);
}

void FileOut::start() {
	file.open(QFile::Truncate | QFile::WriteOnly);
	//file.reset();
	stream.reset();
	stream.setDevice(&file);
    stream << qSetFieldWidth(9);
    stream << qSetRealNumberPrecision(5);
    stream << fixed;
    Logger(Debug) << this << "opening file";
	stream << "time";
	stream << "\t" << QString::fromStdString(Flow::getNames()[0]) << " [m^3/dt]";
	for (int i = 1; i < Flow::getNames().size(); ++i) {
		stream << "\t" << QString::fromStdString(Flow::getNames()[i]) << " [g/m^3]";
	}
	stream << "\n";
}

int FileOut::f(ptime time, int dt) {
	QDateTime d(pttoqt(time));
	stream << d.toString("dd.MM.yyyy hh:mm:ss");

	BOOST_FOREACH(std::string name, Flow::getNames()) {
		stream << "\t" << in.getValue(name);
	}
	stream << "\n";

	return dt;
}

void FileOut::stop() {
	stream.flush();
	file.close();
	Logger(Debug) << this << "closing file";
}
