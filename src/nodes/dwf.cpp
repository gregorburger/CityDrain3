#include "dwf.h"
#include <boost/algorithm/string.hpp>
#include <QFile>
#include <QStringList>
#include <QString>

CD3_DECLARE_NODE_NAME(DWF)

DWF::DWF() {
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(dwf_definition_file));
	addParameter(ADD_PARAMETERS(q_scale));
	q_scale = 1.0;
}

DWF::~DWF() {

}

bool DWF::load(int dt) {
	QString path = QString::fromStdString(dwf_definition_file);
	QFile in(path);
	if (!in.exists()) {
		Logger(Warning) << "no such file or directory " << dwf_definition_file;
		return false;
	}
	in.open(QFile::ReadOnly);
	bool ok;
	while (!in.atEnd()) {
		QString line = in.readLine();
		QString t = line.trimmed();
		if (t.isEmpty()) {
			Logger(Debug) << "found empty line in DWF";
			continue;
		}
		QStringList values = line.split(";");
		if (values.size() < 2) {
			Logger(Warning) << "wrong dwf file format" << line.toStdString();
			return false;
		}
		dwf_entry entry;
		entry.first = values[0].toInt(&ok);
		if (!ok) {
			Logger(Warning) << "wrong dwf file format" << line.toStdString();
			return false;
		}
		if (entry.first % dt != 0) {
			Logger(Warning) << "time spans must be of a multiple of dt";
			return false;
		}
		for (int i = 1; i < values.size(); i++) {
			entry.second.push_back(values[i].toDouble(&ok));
			if (!ok) {
				Logger(Warning) << "wrong dwf file format" << line.toStdString();
				return false;
			}
		}
		//fill up missing concentrations
		while ((entry.second.size() - 1) < Flow::countUnits(Flow::concentration)) {
			entry.second.push_back(0.0);
		}
		dwf.push_back(entry);
	}
	if (dwf.size() == 0) {
		Logger(Warning) << "file contains no dwf information";
		return false;
	}
	return true;
}

bool DWF::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	(void) start;
	if (!load(dt)) {
		return false;
	}
	dwf_work = dwf;
	setOut();
	return true;
}

void DWF::deinit() {
	dwf.clear();
	dwf_work.clear();
}

void DWF::setOut() {
	vector<double> current_dwf = dwf_work.front().second;
	for (size_t i = 0; i < Flow::size(); i++) {
		out[i] = current_dwf[i];
	}
	out[0] = out[0] * q_scale;
}

int DWF::f(ptime time, int dt) {
	if (dwf_work.front().first == 0) {
		dwf_work.pop_front();
		if (dwf_work.empty())
			dwf_work = dwf;
		setOut();
	}
	dwf_work.front().first -= dt;
	return dt;
}
