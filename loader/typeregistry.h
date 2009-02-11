#ifndef TYPEREGISTRY_H
#define TYPEREGISTRY_H

#include <loki/LokiTypeInfo.h>
#include <boost/shared_ptr.hpp>
#include <map>

#include "typefactory.h"

class TypeRegistry {
public:
	template <class T>
	void addTypeFactory(boost::shared_ptr<ITypeFactory> factory) {
		named_types[factory->getTypeName()] = factory;
		typed_types[Loki::TypeInfo(typeid(T))] = factory;
	}

	ITypeFactory *getByTypeName(const std::string &name) {
		return named_types[name].get();
	}

	template <class T>
	ITypeFactory *getByType() {
		return typed_types[Loki::TypeInfo(typeid(T))].get();
	}

private:
	std::map<Loki::TypeInfo, boost::shared_ptr<ITypeFactory> > typed_types;
	std::map<std::string, boost::shared_ptr<ITypeFactory> > named_types;
};

#endif // TYPEREGISTRY_H
