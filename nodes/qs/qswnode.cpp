#include "qswnode.h"

#include <QDebug>
#include <QTime>
#include <string>
#include <boost/format.hpp>

using namespace boost;

#include <flow.h>
#include "qswflow.h"
#include "statemigrator.h"
#include <cd3assert.h>


CD3_DECLARE_NODE_NAME(QSWNode)

struct QSWNodePrivate {
	QScriptEngine engine;
	QStringList in_ports, out_ports;
	QStringList parameters;
	QStringList states;

	QScriptValue f_function;
	QScriptValue init_function;
};

Q_SCRIPT_DECLARE_QMETAOBJECT(QSWFlow, QObject*);

QSWNode::QSWNode(const std::string &s) {
	script_path = s;
	qDebug() << QTime::currentTime() << " QSWNode()";
	priv = new QSWNodePrivate();
	priv->engine.setGlobalObject(priv->engine.newQObject(this));
	QScriptValue flowClass = priv->engine.scriptValueFromQMetaObject<QSWFlow>();
	priv->engine.globalObject().setProperty("Flow", flowClass);

	executeScript(priv->engine);

	cd3assert(priv->engine.globalObject().property("init").isFunction(),
		   "no init function defined");
	cd3assert(priv->engine.globalObject().property("f").isFunction(),
		   "no 'f' function defined");

	priv->init_function = priv->engine.globalObject().property("init");
	priv->f_function = priv->engine.globalObject().property("f");
}

void QSWNode::executeScript(QScriptEngine &engine) {
	QFile f(QString::fromStdString(script_path));
	cd3assert(f.exists(), "script file does not exist");
	cd3assert(f.open(QIODevice::ReadOnly), "could not read open script file");
	QString script = f.readAll();
	QScriptValue script_ret = engine.evaluate(script, f.fileName());
	cd3assert(!script_ret.isError(), script_ret.toString().toStdString());
}

QSWNode::~QSWNode() {
	delete priv;
}

void QSWNode::addInPort(const QString &name, QSWFlow *flow) {
	qDebug() << "addInPort";
	Node::addInPort(name.toStdString(), flow->flow);
}

void QSWNode::addOutPort(const QString &name, QSWFlow *flow) {
	qDebug() << "addOutPort";
	Node::addOutPort(name.toStdString(), flow->flow);
}
;
void QSWNode::addParameter(const QString &name, double dval) {
	priv->parameters.append(name);
	double *d = new double(dval);
	Node::addParameter(name.toStdString(), d);
}

void QSWNode::addParameter(const QString &name, int dval) {
	priv->parameters.append(name);
	int *d = new int(dval);
	Node::addParameter(name.toStdString(), d);
}

void QSWNode::addParameter(const QString &name, QString dval) {
	priv->parameters.append(name);
	std::string *d = new std::string(dval.toStdString());
	Node::addParameter(name.toStdString(), d);
}

void QSWNode::addParameter(const QString &name, bool dval) {
	priv->parameters.append(name);
	bool *d = new bool(dval);
	Node::addParameter(name.toStdString(), d);
}

void QSWNode::addParameter(const QString &name, QSWFlow *flow) {
	priv->parameters.append(name);
	Node::addParameter(name.toStdString(), flow);
}

void QSWNode::addState(const QString &name) {
	priv->states.append(name);
}

void QSWNode::pullOutStates() {
	Q_FOREACH(QString qsname, priv->states) {
		std::string stdname = qsname.toStdString();
		QVariant state_value = priv->engine.globalObject().property(qsname).toVariant();
		cd3assert(
				IStateMigrator::qt.count(state_value.type()) == 1,
				str(format("can not pull state %1%") % stdname));
		IStateMigrator::qt[state_value.type()]->pull(stdname, this, priv->engine);
	}
}

void QSWNode::pushInStates() {
	Q_FOREACH(QString qsname, priv->states) {
		std::string stdname = qsname.toStdString();
		cd3::TypeInfo type = Node::states[stdname].first;
		cd3assert(
				IStateMigrator::cd3.count(type) == 1,
				str(format("can not push state %1%") % stdname));
		IStateMigrator::cd3[type]->push(stdname, this, priv->engine);
	}
}

void QSWNode::pushParameters() {
	Q_FOREACH(QString qsname, priv->parameters) {
		std::string stdname = qsname.toStdString();
		cd3::TypeInfo type = Node::states[stdname].first;
		cd3assert(
				IStateMigrator::cd3.count(type) == 1,
				str(format("can not push parameter %1%") % stdname));
		IStateMigrator::cd3[type]->pushParameter(stdname, this, priv->engine);
	}
}

int QSWNode::f(int time, int dt) {
	QScriptValue f_ret = priv->f_function.call(
			priv->engine.globalObject(),
			QScriptValueList() << time << dt);

	cd3assert(!f_ret.isError(), f_ret.toString().toStdString());

	cd3assert(f_ret.isNumber(), "f must return the calculated delta time");
	return f_ret.toInteger();
}

void QSWNode::init(int start,int end, int dt) {
	pushParameters();
	QScriptValue init_ret = priv->init_function.call(
			priv->engine.globalObject(),
			QScriptValueList() << start << end << dt);
	cd3assert(!init_ret.isError(), init_ret.toString().toStdString());
}

void QSWNode::deinit() {
}
