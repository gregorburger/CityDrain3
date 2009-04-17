#ifndef TYPEFACTORY_H
#define TYPEFACTORY_H

#include <memory>
#include <string>
#include <QDomElement>
#include <boost/shared_ptr.hpp>
#include <cd3globals.h>

class Node;

class CD3_PUBLIC ITypeFactory {
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
class CD3_PUBLIC Type##TypeFactory : public ITypeFactory { \
public: \
	std::string getTypeName() { return #Type; } \
private:


#endif // TYPEFACTORY_H
