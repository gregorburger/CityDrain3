#ifndef QSWCALCUNIT_H
#define QSWCALCUNIT_H

#include <QObject>
#include <calculationunit.h>

class QSWCalcUnit : public QObject
{
	Q_OBJECT
public:
	QSWCalcUnit(CalculationUnit *u, QObject *parent = 0);
	virtual ~QSWCalcUnit();
	CalculationUnit *unit;
};

#endif // QSWCALCUNIT_H
