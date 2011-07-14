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
	Logger(Debug) << this << "opening file";
	stream << qSetFieldWidth(15);
	stream << qSetRealNumberPrecision(15);
	stream << fixed;
	stream << "time";
	BOOST_FOREACH(std::string name, Flow::getNames()) {
		stream << "\t" << QString::fromStdString(name);
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
