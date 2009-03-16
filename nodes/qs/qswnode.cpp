#include "qswnode.h"


#include <QDebug>
#include <QTime>
#include <string>

#include <flow.h>
#include <cd3assert.h>

#include "qswflow.h"


CD3_DECLARE_NODE_NAME(QSWNode)

struct QSWNodePrivate {
	QScriptEngine engine;
};

Q_SCRIPT_DECLARE_QMETAOBJECT(QSWFlow, QObject*);

QSWNode::QSWNode(const std::string &s) {
	script_path = s;
	qDebug() << QTime::currentTime() << " QSWNode()";
	priv = new QSWNodePrivate();
	priv->engine.setGlobalObject(priv->engine.newQObject(this));
	QScriptValue flowClass = priv->engine.scriptValueFromQMetaObject<QSWFlow>();

	priv->engine.globalObject().setProperty("Flow", flowClass);
}

QSWNode::QSWNode() {
	priv = new QSWNodePrivate();
}

QSWNode::~QSWNode() {
	delete priv;
}

const char *QSWNode::getNodeName() const {
	return "QSWNode";
}

void QSWNode::addInPort(const QString &name, QSWFlow *flow) {
	qDebug() << "addInPort";
	Node::addInPort(name.toStdString(), flow->flow);
}

void QSWNode::addOutPort(const QString &name, QSWFlow *flow) {
	qDebug() << "addOutPort";
	Node::addOutPort(name.toStdString(), flow->flow);
}

void QSWNode::quadMe(QScriptValue value) {
	assert(value.isNumber(), "value not a numer");
	int i = value.toInteger();
	qDebug() << "quad me: " << i;
	value = QScriptValue(i*i);
}

void QSWNode::addParameter(const QString &n, bool *b) {
	(void) n;
	(void) b;
	/*qDebug() << QTime::currentTime() << " QSWNode()::addParameter";
	std::string name = n.toStdString();
	if (value.isBoolean()) {
		if (value.toVariant().canConvert<void*>()) {
			qDebug() << "can convert to pointer";
		} else {
			qDebug() << "can not convert to pointer";
		}
	}*/
}

int QSWNode::f(int time, int dt) {
	QScriptValue f_fun_value = priv->engine.globalObject().property("f");

	assert(!f_fun_value.isUndefined(), "no f function defined in script");
	assert(f_fun_value.isFunction(), "f is not a function maybe it was overwritten");
	QScriptValue f_ret = f_fun_value.call(
			priv->engine.globalObject(),
			QScriptValueList() << time << dt);

	assert(!f_ret.isError(), f_ret.toString().toStdString());

	/*QScriptValueIterator it(priv->engine.globalObject());
	while(it.hasNext()) {
		it.next();
		if (it.flags() & QScriptValue::SkipInEnumeration)
			continue;
		qDebug() << "global has: " << it.name();
	}*/

	assert(f_ret.isNumber(), "f did not return a number");
	return f_ret.toInteger();
}

void QSWNode::init(int start,int end, int dt) {
	QFile f(QString::fromStdString(script_path));
	assert(f.exists(), "script file does not exist");
	assert(f.open(QIODevice::ReadOnly), "could not read open script file");
	QString script = f.readAll();
	QScriptValue script_ret = priv->engine.evaluate(script, f.fileName());
	assert(!script_ret.isError(), script_ret.toString().toStdString());

	QScriptValue init_fun_value = priv->engine.globalObject().property("init");
	assert(init_fun_value.isFunction(), "no init function defined in script");
	QScriptValue init_ret = init_fun_value.call(
			priv->engine.globalObject(),
			QScriptValueList() << start << end << dt);

	assert(!init_ret.isError(), init_ret.toString().toStdString());

	/*QScriptValueIterator it(priv->engine.globalObject());
	while(it.hasNext()) {
		it.next();
		if (it.flags() & QScriptValue::SkipInEnumeration)
			continue;
		qDebug() << "global has: " << it.name() << " and is : " << it.value().toString();
	}*/

}

void QSWNode::deinit() {
}
