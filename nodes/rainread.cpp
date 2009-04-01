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
	QDateTime base_date;
	QDateTime file_date;
};

RainRead::RainRead() {
	out.addUnit("rain", CU::rain, 0.0);
	addOutPort(ADD_PARAMETERS(out));
	addParameter(ADD_PARAMETERS(file_name));
	addParameter(ADD_PARAMETERS(base_date));
	rain = 0;
	addState(ADD_PARAMETERS(rain));
	priv = new RainReadPriv();
}

RainRead::~RainRead() {
	delete priv;
}

void RainRead::init(int start, int stop, int dt) {
	(void) start;
	(void) stop;
	priv->rain_file = new QFile(QString::fromStdString(file_name));
	assert(priv->rain_file->exists(), "no such rain file");
	priv->rain_file->open(QIODevice::ReadOnly);

	priv->file_date = parseLine().get<0>();

	if (base_date != "") {
		priv->base_date = QDateTime::fromString(QString::fromStdString(base_date),
												"d.M.yyyy h:m:s");
	} else {
		priv->base_date = priv->file_date;
	}

	priv->rain_file->reset();

	//vorspulen >>
	while (priv->file_date.addSecs(dt) < priv->base_date) {
		QDateTime current_date = parseLine().get<0>();
		priv->file_date = current_date;
	}
	priv->file_date.addSecs(-dt);
}

void RainRead::deinit() {
	delete priv->rain_file;
}

#define TO_D static_cast<double>

int RainRead::f(int time, int dt) {
	QDateTime calc_date = priv->base_date.addSecs(time+dt);
	while (priv->file_date < calc_date) {
		QDateTime current_date;
		double current_rain;
		tie(current_date, current_rain) = parseLine();
		rain += current_rain;
		priv->file_date = current_date;
	}
	assert(calc_date <= priv->file_date, "asdf");

	int rdt = QDateTime(priv->base_date.addSecs(time)).secsTo(priv->file_date);

	double out_rain = TO_D(dt) / TO_D(rdt) * rain;

	rain -= out_rain;

	out.setValue("rain", out_rain);
	return dt;
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
