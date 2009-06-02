#ifndef QSWFLOW_H
#define QSWFLOW_H

#include <QObject>
#include <QtScript>

class Flow;
class QSWCalcUnit;

class QSWFlow : public QObject
{
	Q_OBJECT

public:
	QSWFlow(Flow *f, QObject *parent = 0);
	QSWFlow(QObject *parent = 0);

	Flow *flow;

public Q_SLOTS:
	void clear();
	void addUnit(QString name, QSWCalcUnit *unit, double value);
	void setValue(QString name, double value);
	double getValue(QString name) const;
	void setIth(QSWCalcUnit *unit, int i, double value);
	double getIth(QSWCalcUnit *unit, int i);

	QStringList getNames() const;
	QStringList getUnitNames(QSWCalcUnit *unit) const;


private:
	friend class QSWNode;
};

Q_SCRIPT_DECLARE_QMETAOBJECT(QSWFlow, QObject*);
Q_DECLARE_METATYPE(QSWFlow*);

#endif // QSWFLOW_H
