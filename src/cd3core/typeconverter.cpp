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

#include "typeconverter.h"
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <list>
#include <map>
#include <iostream>
#include <cmath>
#include <node.h>
#include <flow.h>
#ifndef PYTHON_DISABLED
#include <pythonexception.h>
#endif

using namespace cd3;

template<class T>
struct PrimTC : TC {
	PrimTC(const std::string &type_name) : type_name(type_name) {}
	virtual ~PrimTC(){
	}

	std::string getTypeName() const {
		return type_name;
	}

	TypeInfo getTypeInfo() const {
		return TypeInfo(typeid(T));
	}

	std::string toString(const void *value) const {
		T *tvalue = (T *) value;
		return lexical_cast<std::string>(*tvalue);
	}

	std::string toStringExact(const void *value) const {
		return toString(value);
	}

	void fromString(const std::string &s, void *dest) const {
		T *ivalue = (T *) dest;
		*ivalue = lexical_cast<T>(s);
	}

	void fromStringExact(const std::string &s, void *dest) const {
		fromString(s, dest);
	}

	void setParameter(Node *n,
					  const std::string &pname,
					  const std::string &pvalue) const {
		T v;
		fromString(pvalue, &v);
		n->setParameter<T>(pname, v);
	}

	void setParameterExact(Node *n,
						   const std::string &pname,
						   const std::string &pvalue) const {
		T v;
		fromStringExact(pvalue, &v);
		n->setParameter<T>(pname, v);
	}
#ifndef PYTHON_DISABLED
	void updatePythonParameter(PyObject *self, NodeParameter *p) const {
		PyObject *pvalue = toPython(p->value);
		PyObject *__dict__ = PyObject_GetAttrString(self, "__dict__");

		PyDict_SetItemString(__dict__, p->name.c_str(), pvalue);

		if (PyErr_Occurred()) {
			Py_DECREF(__dict__);
			Py_DECREF(pvalue);
			throw PythonException();
		}

		Py_DECREF(__dict__);
		Py_DECREF(pvalue);
	}

	PyObject *toPython(void *) const {
		return Py_None;
	}
#endif

	std::string type_name;
};

template <>
void PrimTC<double>::fromStringExact(const std::string &s, void *dest) const {
	std::istringstream ss(s);
	double fract;
	int exp;
	ss >> fract >> exp;
	*((double*) dest) = ldexp(fract, exp);
}

template <>
std::string PrimTC<double>:: toStringExact(const void *value) const {
	int exp;
	double fract = frexp(*((double *) value), &exp);
	return lexical_cast<std::string>(fract) + " " + lexical_cast<std::string>(exp);
}

#ifndef PYTHON_DISABLED
template <>
PyObject *PrimTC<double>::toPython(void *p) const {
	return PyFloat_FromDouble(*((double*) p));
}

template <>
PyObject *PrimTC<int>::toPython(void *p) const {
#if PY_MAJOR_VERSION >= 3
	return PyLong_FromLong(*((int*) p));
#else
	return PyInt_FromLong(*((int*) p));
#endif
}

template <>
PyObject *PrimTC<bool>::toPython(void *p) const {
	bool b = *(bool*) p;
	return PyBool_FromLong(b);
}

template <>
PyObject *PrimTC<std::string>::toPython(void *p) const {
	std::string s = *((std::string*) p);
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromString(s.c_str());
#else
	return PyString_FromString(s.c_str());
#endif
}
#endif

struct FlowTC : public TypeConverter {
	~FlowTC() {}
	std::string getTypeName() const {
		return "flow";
	}
	cd3::TypeInfo getTypeInfo() const {
		return cd3::TypeInfo(typeid(Flow));
	}

	std::string toString(const void *value) const {
		Flow *f = (Flow *) value;
		std::string result;
		BOOST_FOREACH(std::string name, Flow::getNames()) {
			result += ";" + name + ";" + lexical_cast<std::string>(f->getValue(name));
		}
		return result.size() > 0 ? result.substr(1) : "";
	}

	std::string toStringExact(const void *value) const {
		PrimTC<double> double_con("");
		Flow *f = (Flow *) value;
		std::string result;
		BOOST_FOREACH(std::string name, Flow::getNames()) {
			double dvalue = f->getValue(name);
			result += ";" + name + ";" + double_con.toStringExact(&dvalue);
		}
		return result.size() > 0 ? result.substr(1) : "";
	}

	void fromString(const std::string &s, void *dest) const {
		Flow *f = (Flow *) dest;
		std::vector<std::string> items;
		split(items, s, algorithm::is_any_of(";"));
		cd3assert(items.size() == Flow::size() * 2, "wrong std::string format of type Flow");
		for (size_t i = 0; i < Flow::size()*2; i += 2) {
			std::string name = items[i];
			double value = lexical_cast<double>(items[i+1]);
			f->setValue(name, value);
		}
	}

	void fromStringExact(const std::string &s, void *dest) const {
		PrimTC<double> double_con("");
		Flow *f = (Flow *) dest;
		std::vector<std::string> items;
		split(items, s, algorithm::is_any_of(";"));
		cd3assert(items.size() == Flow::size() / 2, "wrong std::string format of type Flow");
		for (size_t i = 0; i < Flow::size(); i++) {
			std::string name = items[i*2];
			double value;
			double_con.fromStringExact(items[i*2+1], &value);
			f->setValue(name, value);
		}
	}

	void setParameter(Node *n,
					  const std::string &pname,
					  const std::string &pvalue) const {
		Flow f;
		fromString(pvalue, &f);
		n->setParameter(pname, f);
	}

	void setParameterExact(Node *n,
						   const std::string &pname,
						   const std::string &pvalue) const {
		Flow f;
		fromStringExact(pvalue, &f);
		n->setParameter(pname, f);
	}
#ifndef PYTHON_DISABLED
	PyObject* toPython(void *np) const {
		return Py_None;
	}

	void updatePythonParameter(PyObject *, NodeParameter *) const {

	}
#endif
};

template <class T>
struct ArrayTC : public TypeConverter {
	ArrayTC(std::string type_name) : type_name(type_name) {
	}

	virtual ~ArrayTC() {
	}

	std::string getTypeName() const {
		return "array[" + type_name + "]";
	}

	cd3::TypeInfo getTypeInfo() const {
		return TypeInfo(typeid(std::vector<T>));
	}

	std::string toString(const void *values) const {
		TypeConverter *sub_con = TypeConverter::get(TypeInfo(typeid(T)));
		std::string result;
		BOOST_FOREACH(T value, (*(std::vector<T> *) values)) {
			result += ";" + sub_con->toString(&value);
		}
		return result.size() > 0 ? result.substr(1) : "";
	}

	std::string toStringExact(const void *values) const {
		TypeConverter *sub_con = TypeConverter::get(TypeInfo(typeid(T)));
		std::string result;
		BOOST_FOREACH(T value, (*(std::vector<T> *) values)) {
			result += ";" + sub_con->toString(&value);
		}
		return result.size() > 0 ? result.substr(1) : "";
	}

	void fromString(const std::string &s, void *dest) const {
		TypeConverter *sub_con = TypeConverter::get(TypeInfo(typeid(T)));
		std::vector<T> *values = (std::vector<T> *) dest;
		std::vector<std::string> items;
		split(items, s, algorithm::is_any_of(";"));
		values->clear();
		BOOST_FOREACH(std::string item, items) {
			T value;
			try {
				sub_con->fromString(item, &value);
				values->push_back(value);
			} catch (bad_lexical_cast e) {
			}
		}
	}

	void fromStringExact(const std::string &s, void *dest) const {
		TypeConverter *sub_con = TypeConverter::get(TypeInfo(typeid(T)));
		std::vector<T> *values = (std::vector<T> *) dest;
		std::vector<std::string> items;
		split(items, s, algorithm::is_any_of(";"));
		values->clear();
		BOOST_FOREACH(std::string item, items) {
			T value;
			try {
				sub_con->fromString(item, &value);
				values->push_back(value);
			} catch (bad_lexical_cast e) {
			}
		}
	}

	void setParameter(Node *n,
					  const std::string &pname,
					  const std::string &pvalue) const {
		std::vector<T> v;
		fromString(pvalue, &v);
		n->setParameter(pname, v);
	}

	void setParameterExact(Node *n,
						   const std::string &pname,
						   const std::string &pvalue) const {
		std::vector<T> v;
		fromStringExact(pvalue, &v);
		n->setParameter(pname, v);
	}

#ifndef PYTHON_DISABLED
	void updatePythonParameter(PyObject *self, NodeParameter *p) const {
		PyObject *pvalue = toPython(p->value);
		PyObject *__dict__ = PyObject_GetAttrString(self, "__dict__");

		PyDict_SetItemString(__dict__, p->name.c_str(), pvalue);

		if (PyErr_Occurred()) {
			Py_DECREF(__dict__);
			Py_DECREF(pvalue);
			throw PythonException();
		}

		Py_DECREF(__dict__);
		Py_DECREF(pvalue);
	}

	PyObject* toPython(void *np) const {
		TypeConverter *sub_con = TypeConverter::get(TypeInfo(typeid(T)));
		std::vector<T> v = *((std::vector<T>*) np);
		PyObject *list = PyList_New(0);
		BOOST_FOREACH(T t, v) {
			PyList_Append(list, sub_con->toPython(&t));
		}
		return list;
	}
#endif
	std::string type_name;
};

#define NEW_PRIMTC(type) (TypeConverter *) new PrimTC<type>(#type)
#define NEW_ARRAYTC(type) (TypeConverter *) new ArrayTC<type>(#type)

std::string TypeConverter::toStringExact(const void *value) const {
	return toString(value);
}

void TypeConverter::fromStringExact(const std::string &s, void *dest) const {
	return fromString(s, dest);
}

TypeConverter *TypeConverter::get(const cd3::TypeInfo &type) {
	static bool once = true;
	static std::map<TypeInfo, TypeConverter *> cmap;
	if (once) {
		once = false;
		BOOST_FOREACH(TypeConverter *c, converters) {
			cmap[c->getTypeInfo()] = c;
		}
	}
	return cmap[type];
}

TypeConverter *TypeConverter::get(const std::string &name) {
	static bool once = true;
	static std::map<std::string, TypeConverter *> cmap;
	if (once) {
		once = false;
		BOOST_FOREACH(TypeConverter *c, converters) {
			cmap[c->getTypeName()] = c;
		}

	}
	return cmap[name];
}

using namespace std;

list<TypeConverter *> TypeConverter::converters = assign::list_of
										  (NEW_PRIMTC(int))
										  (NEW_PRIMTC(float))
										  (NEW_PRIMTC(string))
										  (NEW_PRIMTC(double))
										  (new FlowTC())
										  (NEW_ARRAYTC(double))
										  (NEW_ARRAYTC(bool))
										  (NEW_ARRAYTC(int))
										  (NEW_ARRAYTC(string))
										  ;
