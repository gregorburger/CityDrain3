#ifndef TYPECONVERTER_H
#define TYPECONVERTER_H

#include <cd3typeinfo.h>
#include <cd3globals.h>
#include <string>
#include <list>

class Node;

class CD3_PUBLIC TypeConverter {
public:
	static TypeConverter *get(const cd3::TypeInfo &type);
	static TypeConverter *get(const std::string &name);

	virtual ~TypeConverter(){}

	virtual std::string getTypeName() const = 0;
	virtual cd3::TypeInfo getTypeInfo() const = 0;

	virtual std::string toString(const void *) const = 0;
	virtual std::string toStringExact(const void *) const;
	virtual void fromString(const std::string &s, void *dest) const = 0;
	virtual void fromStringExact(const std::string &s, void *dest) const;

	virtual void setParameter(Node *n,
							  const std::string &pname,
							  const std::string &pvalue) const = 0;
	static std::list<TypeConverter *> converters;
};

typedef TypeConverter TC;

#endif // TYPECONVERTER_H
