#include "controller.h"

#include "model.h"
#include "simulation.h"
#include "node.h"
#include "flow.h"

#include <QtScript>

using namespace std;

struct ControllerPrivate {
	QScriptEngine engine;
	QScriptValue beforeFun;
	QScriptValue afterFun;
	ISimulation *simulation;
};

Controller::Controller(const string &script) {
	(void) script;
	priv = new ControllerPrivate();
	priv->engine.setGlobalObject(priv->engine.newQObject(this));
	QScriptValue flowClass = priv->engine.scriptValueFromQMetaObject<QSWFlow>();
	priv->engine.globalObject().setProperty("Flow", flowClass);

	executeScript(script);

	cd3assert(priv->engine.globalObject().property("controllAfter").isFunction(),
		   "no 'controllAfter' function defined");
	cd3assert(priv->engine.globalObject().property("controllBefore").isFunction(),
		   "no 'controllBefore' function defined");

	priv->afterFun = priv->engine.globalObject().property("controllAfter");
	priv->beforeFun = priv->engine.globalObject().property("controllBefore");
}

Controller::~Controller() {
	delete priv;
}

void Controller::controllBefore(ISimulation *simulation, int time) {
	cd3assert(simulation, "simulation null");
	cd3assert(priv->beforeFun.isFunction(), "no such function: beforeFun");
	priv->simulation = simulation;
	QScriptValue ret = priv->beforeFun.call(time);
	cd3assert(!ret.isError(), ret.toString().toStdString())
}

void Controller::controllAfter(ISimulation *simulation, int time) {
	cd3assert(simulation, "simulation null");
	cd3assert(priv->afterFun.isFunction(), "no such function: afterFun");
	priv->simulation = simulation;
	QScriptValue ret = priv->afterFun.call(time);
	cd3assert(!ret.isError(), ret.toString().toStdString())
}

void Controller::executeScript(const string &s) {
	QFile f(QString::fromStdString(s));
	cd3assert(f.exists(), "script file does not exist");
	cd3assert(f.open(QIODevice::ReadOnly), "could not read open script file");
	QString script = f.readAll();
	QScriptValue script_ret = priv->engine.evaluate(script, f.fileName());
	cd3assert(!script_ret.isError(), script_ret.toString().toStdString());
}

void Controller::stopSimulation() {
	priv->simulation->stop();
}

void Controller::serialize(QString dir) {
	priv->simulation->serialize(dir.toStdString());
}

void Controller::deserialize(QString dir, int time) {
	priv->simulation->deserialize(dir.toStdString(), time);
}

void Controller::setInt(
				const QString &node,
				const QString &state,
				int &value) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	n->setState<int>(state.toStdString(), value);
}

int *Controller::getInt(
			   const QString &node,
			   const QString &state) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	return n->getState<int>(state.toStdString());
}

void Controller::setDouble(
				const QString &node,
				const QString &state,
				double &value) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	n->setState<double>(state.toStdString(), value);
}

double *Controller::getDouble(
			   const QString &node,
			   const QString &state) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	return n->getState<double>(state.toStdString());
}

void Controller::setString(
				const QString &node,
				const QString &state,
				string &value) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	n->setState<string>(state.toStdString(), value);
}

string *Controller::getString(
			   const QString &node,
			   const QString &state) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	return n->getState<string>(state.toStdString());
}

void Controller::setBool(
				const QString &node,
				const QString &state,
				bool &value) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	n->setState<bool>(state.toStdString(), value);
}

bool *Controller::getBool(
			   const QString &node,
			   const QString &state) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	return n->getState<bool>(state.toStdString());
}

void Controller::setFlow(
				const QString &node,
				const QString &state,
				QSWFlow &value) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	n->setState<Flow>(state.toStdString(), *value.flow);
}

QObject *Controller::getFlow(
			   const QString &node,
			   const QString &state) {
	Node *n = priv->simulation->getModel()->getNode(node.toStdString());
	return new QSWFlow(n->getState<Flow>(state.toStdString()));
}

void Controller::print(QScriptValue v) {
	qDebug() << v.toString();
}
