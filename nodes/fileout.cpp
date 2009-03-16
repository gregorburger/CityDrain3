#include "fileout.h"

#include <flow.h>
#include <QFile>
#include <QTextStream>
#include <boost/foreach.hpp>

CD3_DECLARE_NODE_NAME(FileOut)

FileOut::FileOut()
	: Node(), stream(&file)
{
	in = new Flow();
	out_file_name = new std::string();
	addParameter(ADD_PARAMETERS_P(out_file_name));
	addInPort(ADD_PARAMETERS_P(in));
}

FileOut::~FileOut() {
	//std::cout << "closing file" << std::endl;
	delete out_file_name;
	delete in;
	if (file.isOpen())
		file.close();
}

void FileOut::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	assert(out_file_name);
	file.setFileName(QString::fromStdString(*out_file_name));
	if (!file.isOpen()) {
		file.open(QFile::WriteOnly);
	}
	stream.setRealNumberPrecision(10);
}

void FileOut::deinit() {
	//file->close();
}

int FileOut::f(int time, int dt) {
	stream << qSetFieldWidth(15);
	stream << fixed;
	static bool first = true;
	if (first) {
		stream << "time";
		BOOST_FOREACH(std::string name, in->getNames()) {
			stream << "\t" << QString::fromStdString(name);
		}
		stream << endl;
		first = false;
	}
	stream << time;

	BOOST_FOREACH(std::string name, in->getNames()) {
			stream << "\t" << in->getValue(name);
	}
	stream << endl;
	return dt;
}
