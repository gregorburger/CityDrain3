#ifndef QSWCALCUNIT_H
#define QSWCALCUNIT_H

#include <QObject>
#include <QtScript>

#include <cd3globals.h>
#include <flow.h>

class CD3_PUBLIC QSWCalcUnit : public QObject
{
	Q_OBJECT
public:
	QSWCalcUnit(QObject * = 0);
	QSWCalcUnit(Flow::CalculationUnit u, QObject *parent = 0);
	virtual ~QSWCalcUnit();
	static void setGlobalUnits(QScriptEngine &engine);
	Flow::CalculationUnit unit;
};

Q_SCRIPT_DECLARE_QMETAOBJECT(QSWCalcUnit, QObject*);
Q_DECLARE_METATYPE(QSWCalcUnit*);

#endif // QSWCALCUNIT_H
