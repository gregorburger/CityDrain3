#include "rainread.h"
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>

#include <calculationunit.h>

#include <cd3assert.h>

CD3_DECLARE_NODE_NAME(RainRead)

struct RainReadPriv {
	QFile *rain_file;
	int delta;
};

RainRead::RainRead() {
	out.addUnit("rain", CU::rain, 0.0);
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(file_name));
	priv = new RainReadPriv();
}

RainRead::~RainRead() {
	delete priv;
}

void RainRead::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	(void) dt;
	priv->rain_file = new QFile(QString::fromStdString(file_name));
	assert(priv->rain_file->exists(), "no such rain file");
	priv->rain_file->open(QIODevice::ReadOnly);

	QDateTime first = parseLine().get<0>();
	QDateTime second = parseLine().get<0>();
	priv->delta = first.secsTo(second);
	priv->rain_file->reset();
}

void RainRead::deinit() {
	delete priv->rain_file;
}

int RainRead::f(int time, int dt) {
	(void) time;
	(void) dt;

	double rr = parseLine().get<1>();
	out.clear();
	out.setValue("rain", rr);
	return priv->delta;
}

tuple<QDateTime, double> RainRead::parseLine() {
	QString line = priv->rain_file->readLine();
	QStringList splitted = line.split(QRegExp("[ \\t]+"));
	assert(splitted.count() == 3, "wrong rain file format");

	double rr = splitted[2].toDouble();
	QString date_time = QString("%1 %2").arg(splitted[0]).arg(splitted[1]);
	QDateTime date = QDateTime::fromString(date_time, "dd.MM.yyyy hh:mm:ss");

	return tuple<QDateTime, double>(date, rr);
}
