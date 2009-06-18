#include "qswcalcunit.h"
#include <calculationunit.h>

#include <QDebug>

QSWCalcUnit::QSWCalcUnit(QObject *parent)
	: QObject(parent) {
}

QSWCalcUnit::QSWCalcUnit(CalculationUnit *unit, QObject *parent)
 : QObject(parent), unit(unit) {
}

QSWCalcUnit::~QSWCalcUnit() {
}

void QSWCalcUnit::setGlobalUnits(QScriptEngine &engine) {
	QScriptValue flow = engine.newQObject(new QSWCalcUnit(CalculationUnit::flow));
	engine.globalObject().setProperty("flow", flow);

	QScriptValue concentration = engine.newQObject(new QSWCalcUnit(CalculationUnit::concentration));
	engine.globalObject().setProperty("concentration", concentration);

	QScriptValue rain = engine.newQObject(new QSWCalcUnit(CalculationUnit::rain));
	engine.globalObject().setProperty("rain", rain);
}
