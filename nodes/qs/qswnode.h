#ifndef QSWNODE_H
#define QSWNODE_H

#include <QObject>
#include <node.h>

#include <QtScript>

class QSWFlow;
class QSWNodePrivate;
class Flow;

class QSWNode : public QObject, public Node, protected QScriptable {
	Q_OBJECT
public:
	static const char *name;
	QSWNode(const std::string &script);
	~QSWNode();
	const char *getClassName() const;
	int f(int time, int dt);
	void init(int start, int end, int dt);
	void deinit();

public Q_SLOTS:
	void addInPort(const QString &name, QSWFlow *flow);
	void addOutPort(const QString &name, QSWFlow *flow);
	void addParameter(const QString &name, double);
	void addParameter(const QString &name, int);
	void addParameter(const QString &name, QString);
	void addParameter(const QString &name, bool);
	void addParameter(const QString &name, QSWFlow *flow);

	void addState(const QString &name);

	void pushInStates();
	void pullOutStates();

	void print(QScriptValue v);

private:
	void executeScript(QScriptEngine &engine);
	void pushParameters();

	std::string script_path;
	QSWNodePrivate *priv;
};

#endif // QSWNODE_H
