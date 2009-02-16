#include "fileout.h"

#include <flow.h>
#include <QFile>
#include <QTextStream>
#include <boost/foreach.hpp>

FileOut::FileOut()
	: Node()
{
	addParameter(ADD_PARAMETERS_P(out_file_name));
	addInPort(ADD_PARAMETERS_P(in));
}

FileOut::~FileOut() {
	delete file;
}

void FileOut::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	assert(out_file_name);
	file = new QFile(out_file_name->c_str());
	stream = new QTextStream(file);
}

void FileOut::deinit() {
	file->close();
}

void FileOut::f(int time, int dt) {
	(void) time;
	(void) dt;
	(*stream) << time;
	for (int i = 0; i < in->getSize(); i++) {
		(*stream) << "\t" << in->const_data[i];
	}
	(*stream) << endl;
}
