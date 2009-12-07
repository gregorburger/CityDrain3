#include "qswflow.h"

#include <flow.h>
#include <QDebug>
#include <QTime>
#include <boost/foreach.hpp>

#include "qswcalcunit.h"
#include <cd3assert.h>

QSWFlow::QSWFlow(Flow *f, QObject *parent)
 : QObject(parent), flow(f) {
}

QSWFlow::QSWFlow(QObject *parent)
 : QObject(parent) {
	flow = new Flow();
}

void QSWFlow::clear() {
	flow->clear();
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

void QSWFlow::copy(QSWFlow *other) {
	*flow = *other->flow;
}
