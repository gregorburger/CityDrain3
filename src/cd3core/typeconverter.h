/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#ifndef TYPECONVERTER_H
#define TYPECONVERTER_H
#ifndef PYTHON_DISABLED
#include <Python.h>
#endif
#include <cd3typeinfo.h>
#include <cd3globals.h>
#include <string>
#include <list>

class Node;
struct NodeParameter;

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
	virtual void setParameterExact(Node *n,
								   const std::string &pname,
								   const std::string &pvalue) const = 0;
#ifndef PYTHON_DISABLED
	virtual void updatePythonParameter(PyObject *self,
									   NodeParameter *p) const = 0;

	virtual PyObject *toPython(void *) const = 0;
#endif
	static std::list<TypeConverter *> converters;
};

typedef TypeConverter TC;

#endif // TYPECONVERTER_H
