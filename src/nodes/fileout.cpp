#include "fileout.h"

#include <flow.h>
#include <logger.h>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <boost/foreach.hpp>

CD3_DECLARE_NODE_NAME(FileOut)

FileOut::FileOut()
	: Node(), stream(&file)
{
	out_file_name = "/tmp/out.txt";
	addParameter(ADD_PARAMETERS(out_file_name));
	addInPort(ADD_PARAMETERS(in));
}

FileOut::~FileOut() {
}

bool FileOut::init(ptime start, ptime stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	if (out_file_name == "") {
		Logger(Warning) << "out_file_name not defined";
		return false;
	}
	QString q_out_file_name = QString::fromStdString(out_file_name);
	if (QFile::exists(q_out_file_name)) {
		Logger(Warning) << "overwriting file " << out_file_name;
	}
	QFileInfo info(q_out_file_name);
	if (info.absoluteDir().exists()) {
		Logger(Warning) << "parent dir of" << out_file_name << "does not exist";
		return false;
	}
	file.setFileName(q_out_file_name);
	if (!file.isOpen()) {
		file.open(QFile::WriteOnly);
	}
	Logger() << this << "using fileout:" << out_file_name;
	stream.setRealNumberPrecision(10);
	return true;
}

void FileOut::deinit() {
	if (file.isOpen())
		file.close();
}

int FileOut::f(ptime time, int dt) {
	static bool first_run = true;
	if (first_run) {
		stream << qSetFieldWidth(15);
		stream << fixed;
		stream << "time";
		BOOST_FOREACH(std::string name, Flow::getNames()) {
			stream << "\t" << QString::fromStdString(name);
		}
		stream << "\n";
		first_run = false;
	}
	stream << to_simple_string(time).c_str();

	BOOST_FOREACH(std::string name, Flow::getNames()) {
			stream << "\t" << in.getValue(name);
	}
	stream << "\n";
	return dt;
}
