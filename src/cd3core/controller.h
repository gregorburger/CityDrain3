#ifndef ICONTROLLER_H
#define ICONTROLLER_H

#include <qswflow.h>

#include <string>
#include <QObject>
using namespace std;

class IModel;
class ISimulation;
class ControllerPrivate;

class Controller : public QObject
{
	Q_OBJECT
public:
	Controller(const string &script);
	virtual ~Controller();
	void controllBefore(ISimulation *s, int time);
	void controllAfter(ISimulation *s, int time);

	void executeScript(const string &script);

public Q_SLOTS:
	void stopSimulation();

	void serialize(QString dir);
	void deserialize(QString dir, int time);

	void setInt(
				const QString &node,
				const QString &state,
				int &value);
	int *getInt(
			   const QString &node,
			   const QString &state);
	void setDouble(
				const QString &node,
				const QString &state,
				double &value);
	double *getDouble(
			   const QString &node,
			   const QString &state);
	void setString(
				const QString &node,
				const QString &state,
				string &value);
	string *getString(
			   const QString &node,
			   const QString &state);
	void setBool(
				const QString &node,
				const QString &state,
				bool &value);
	bool *getBool(
			   const QString &node,
			   const QString &state);
	void setFlow(
				const QString &node,
				const QString &state,
				QSWFlow &value);
	QObject *getFlow(
			   const QString &node,
			   const QString &state);

	void print(QScriptValue v);

private:
	ControllerPrivate *priv;
};

#endif // ICONTROLLER_H
