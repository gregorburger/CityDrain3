#ifndef ISTATEMIGRATOR_H
#define ISTATEMIGRATOR_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <QVariant>

namespace cd3 {
	struct TypeInfo;
}

class Node;
struct IStateMigrator;
class QScriptEngine;

typedef std::map<cd3::TypeInfo, boost::shared_ptr<IStateMigrator> > cd3_to_ism;
typedef std::map<QVariant::Type, boost::shared_ptr<IStateMigrator> > qt_to_ism;

class IStateMigrator
{
public:
	virtual ~IStateMigrator(){}
	virtual void pull(const std::string &name, Node *, QScriptEngine &engine) = 0;
	virtual void push(const std::string &name, Node *, QScriptEngine &engine) = 0;
	virtual void pushParameter(const std::string &name, Node *, QScriptEngine &engine) = 0;

	static cd3_to_ism cd3;
	static qt_to_ism qt;
};

#endif // ISTATEMIGRATOR_H
