#include "fileout.h"

#include <flow.h>
#include <QFile>
#include <QTextStream>
#include <boost/foreach.hpp>

FileOut::FileOut()
	: Node()
{
	in = new Flow();
	out_file_name = new std::string();
	addParameter(ADD_PARAMETERS_P(out_file_name));
	addInPort(ADD_PARAMETERS_P(in));
}

FileOut::~FileOut() {
	//file->close();
	if (file && file->isOpen()) {
		file->close();
		delete file;
	}
}

void FileOut::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	assert(out_file_name);
	file = new QFile(out_file_name->c_str());
	std::cout << "FileOut::init " << *out_file_name << std::endl;
	stream = new QTextStream(file);
}

void FileOut::deinit() {
	//file->close();
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
