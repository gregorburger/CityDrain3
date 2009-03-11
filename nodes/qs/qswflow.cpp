#include "qswflow.h"

#include <flow.h>
#include <QDebug>
#include <QTime>

#include "qswcalcunit.h"
#include <boost/foreach.hpp>

QSWFlow::QSWFlow(Flow *f, QObject *parent)
 : QObject(parent), flow(f) {
	qDebug() << QTime::currentTime() << " QSWFlow(Flow)";
}

QSWFlow::QSWFlow(QObject *parent)
 : QObject(parent) {
	flow = new Flow();
	qDebug() << QTime::currentTime() << " QSWFlow()";
}

void QSWFlow::clear() {
	qDebug() << QTime::currentTime() << "QSWFlow::clear()";
	flow->clear();
}

void QSWFlow::addUnit(QString name, QSWCalcUnit *unit, double value) {
	flow->addUnit(name.toStdString(), unit->unit, value);
}

void QSWFlow::setValue(QString name, double value) {
	flow->setValue(name.toStdString(), value);
}

double QSWFlow::getValue(QString name) const {
	return flow->getValue(name.toStdString());
}

void QSWFlow::setIth(QSWCalcUnit *unit, int i, double value) {
	flow->setIth(unit->unit, i, value);
}

double QSWFlow::getIth(QSWCalcUnit *unit, int i) {
	return flow->getIth(unit->unit, i);
}

QStringList QSWFlow::getNames() const {
	QStringList names;
	BOOST_FOREACH(std::string name, flow->getNames()) {
		names << QString::fromStdString(name);
	}
	return names;
}

QStringList QSWFlow::getUnitNames(QSWCalcUnit *unit) const {
	QStringList names;
	BOOST_FOREACH(std::string name, flow->getUnitNames(unit->unit)) {
		names << QString::fromStdString(name);
	}
	return names;
}
