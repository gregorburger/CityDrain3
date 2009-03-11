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
	QSWNode(const std::string &script);
	QSWNode();
	~QSWNode();
	const char *getNodeName() const;
	int f(int time, int dt);
	void init(int start, int end, int dt);
	void deinit();

public slots:
	void addInPort(const QString &name, QSWFlow *flow);
	void addOutPort(const QString &name, QSWFlow *flow);
	void addParameter(const QString &name, bool *b);
	void quadMe(QScriptValue value);
private:
	std::string script_path;
	QSWNodePrivate *priv;
	//Flow *in_out;
};

#endif // QSWNODE_H
