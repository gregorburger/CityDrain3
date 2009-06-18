#include "statemigrator.h"

#include <cd3typeinfo.h>
#include <node.h>
#include <flow.h>
#include "qswflow.h"

#include <QtScript>
#include <boost/assign/list_of.hpp>

using namespace boost;
using namespace std;




template <class T>
struct BasicStateMigrator : public IStateMigrator {
	void pull(const string &name, Node *node, QScriptEngine &engine) {
		QVariant var = engine.globalObject().property(QString::fromStdString(name)).toVariant();
		T val = var.value<T>();
		if (node->const_states->find(name) == node->const_states->end()) {
			T *pval = new T(val);
			node->addState(name, pval);
		} else {
			node->setState(name, val);
		}
	}

	void push(const string &name, Node *node, QScriptEngine &engine) {
		QScriptValue v(*node->getState<T>(name));
		engine.globalObject().setProperty(QString::fromStdString(name), v);
	}

	void pushParameter(const string &name, Node *node, QScriptEngine &engine) {
		QScriptValue v(*node->getParameter<T>(name));
		engine.globalObject().setProperty(QString::fromStdString(name), v);
	}
};

struct StrStateMigrator : public IStateMigrator {
	void pull(const string &name, Node *node, QScriptEngine &engine) {
		QString var = engine.globalObject().property(QString::fromStdString(name)).toString();
		string std_var = var.toStdString();
		if (node->const_states->find(name) == node->const_states->end()) {
			string *pstring = new string(std_var);
			node->addState(name, pstring);
		} else {
			node->setState(name, std_var);
		}
	}

	void push(const string &name, Node *node, QScriptEngine &engine) {
		string std_val = *node->getState<string>(name);
		QString s = QString::fromStdString(std_val);
		engine.globalObject().setProperty(QString::fromStdString(name), s);
	}

	void pushParameter(const string &name, Node *node, QScriptEngine &engine) {
		string std_val = *node->getParameter<string>(name);
		QString s = QString::fromStdString(std_val);
		engine.globalObject().setProperty(QString::fromStdString(name), s);
	}
};

struct FlowStateMigrator : public IStateMigrator {
	void pull(const string &name, Node *node, QScriptEngine &engine) {
		QString qname = QString::fromStdString(name);

		QSWFlow *f = qobject_cast<QSWFlow*>(engine.globalObject().property(qname).toQObject());
		if (node->const_states->find(name) == node->const_states->end()) {
			node->addState(name, f->flow);
		} else {
			node->setState(name, *f->flow);
		}
	}

	void push(const string &name, Node *node, QScriptEngine &engine) {
		Flow *f = node->getState<Flow>(name);

		QSWFlow *qswf = new QSWFlow(f);
		QScriptValue flow_value = engine.newQObject(qswf);
		engine.globalObject().setProperty(QString::fromStdString(name), flow_value);
	}

	void pushParameter(const string &name, Node *node, QScriptEngine &engine) {
		Flow *f = node->getParameter<Flow>(name);

		QSWFlow *qswf = new QSWFlow(f);
		QScriptValue flow_value = engine.newQObject(qswf);
		engine.globalObject().setProperty(QString::fromStdString(name), flow_value);
	}
};

typedef shared_ptr<IStateMigrator> spsm;

cd3_to_ism IStateMigrator::cd3 = assign::map_list_of
	(cd3::TypeInfo(typeid(double)), spsm(new BasicStateMigrator<double>()))
	(cd3::TypeInfo(typeid(bool)), spsm(new BasicStateMigrator<bool>()))
	(cd3::TypeInfo(typeid(int)), spsm(new BasicStateMigrator<int>()))
	(cd3::TypeInfo(typeid(string)), spsm(new StrStateMigrator()))
	(cd3::TypeInfo(typeid(Flow)), spsm(new FlowStateMigrator()));


qt_to_ism IStateMigrator::qt = assign::map_list_of
	(QVariant::Double, spsm(new BasicStateMigrator<double>()))
	(QVariant::Bool, spsm(new BasicStateMigrator<bool>()))
	(QVariant::Int, spsm(new BasicStateMigrator<int>()))
	(QVariant::String, spsm(new StrStateMigrator()))
	(QVariant::UserType, spsm(new FlowStateMigrator()));
