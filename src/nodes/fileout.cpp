#include "fileout.h"

#include <flow.h>
#include <logger.h>
#include <QFile>
#include <QTextStream>
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

void FileOut::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	cd3assert(out_file_name.size(), "out_file_name not defined");
	file.setFileName(QString::fromStdString(out_file_name));
	if (!file.isOpen()) {
		file.open(QFile::WriteOnly);
	}
	Logger() << this << "using fileout:" << out_file_name;
	stream.setRealNumberPrecision(10);
}

void FileOut::deinit() {
	if (file.isOpen())
		file.close();
}

int FileOut::f(int time, int dt) {
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
	stream << time;

	BOOST_FOREACH(std::string name, Flow::getNames()) {
			stream << "\t" << in.getValue(name);
	}
	stream << "\n";
	return dt;
}
