#ifndef TYPEFACTORY_H
#define TYPEFACTORY_H

#include <memory>
#include <string>
#include <QDomElement>
#include <boost/shared_ptr.hpp>

class Node;

class ITypeFactory {
public:
	virtual std::string getTypeName() = 0;

	virtual void setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element) = 0;

	virtual void setState(Node *n,
						  const std::string &parameter,
						  QDomElement &element) = 0;
};


#define CD3_DECLARE_TYPE_FACTORY(Type) \
class Type##TypeFactory : public ITypeFactory { \
public: \
	std::string getTypeName() { return #Type; } \
private:


#endif // TYPEFACTORY_H
