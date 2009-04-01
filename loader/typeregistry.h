#ifndef TYPEREGISTRY_H
#define TYPEREGISTRY_H

#include <map>
#include <boost/foreach.hpp>

#include "typefactory.h"

typedef std::map<cd3::TypeInfo, ITypeFactory *> tttype;
typedef std::map<std::string, ITypeFactory *> nttype;

class TypeRegistry {
public:

	~TypeRegistry() {
		typedef std::pair<cd3::TypeInfo, ITypeFactory *> ptype;
		BOOST_FOREACH(ptype t, typed_types) {
			delete t.second;
		}
	}

	template <class T>
	void addTypeFactory(ITypeFactory *factory) {
		named_types[factory->getTypeName()] = factory;
		typed_types[cd3::TypeInfo(typeid(T))] = factory;
	}

	ITypeFactory *getByTypeName(const std::string &name) {
		return named_types[name];
	}

	template <class T>
	ITypeFactory *getByType() {
		return typed_types[cd3::TypeInfo(typeid(T))];
	}

private:
	tttype typed_types;
	nttype named_types;
};

#endif // TYPEREGISTRY_H
