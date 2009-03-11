#include "qswcalcunit.h"

QSWCalcUnit::QSWCalcUnit(CalculationUnit *unit, QObject *parent)
 : QObject(parent), unit(unit) {
}

QSWCalcUnit::~QSWCalcUnit() {
}
