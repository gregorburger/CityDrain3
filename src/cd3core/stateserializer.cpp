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

#include "stateserializer.h"
#include "node.h"
#include "flow.h"
#include "cd3typeinfo.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <sstream>
#include <cmath>
#include <cd3assert.h>

template <class T>
struct BasicSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, T>(*node->getState<T>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		T Tval = boost::lexical_cast<T>(value);
		node->setState<T>(name, Tval);
	}
};

struct FlowSer : public IStateSerializer {
		std::string serialize(const std::string &name, Node *node) {
		return FlowSerializer::toString(*node->getState<Flow>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name, Node *node) {
		Flow f = FlowSerializer::fromString(value);
		node->setState<Flow>(name, f);
	}
};

struct StringSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return *node->getState<std::string>(name);
	}

	void deserialize(const std::string &invalue,
					 const std::string &name,  Node *node) {
		std::string value = invalue;
		node->setState<std::string>(name, value);
	}
};

struct FPSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		std::ostringstream stream;
		int exp;
		double r = *node->getState<double>(name);
		double fract = std::frexp(r, &exp);
		stream << fract << " " << exp;
		return stream.str();
	}
	void deserialize(const std::string &invalue,
			 const std::string &name,  Node *node) {
		std::istringstream stream(invalue);
		double fract;
		int exp;
		stream >> fract >> exp;
		double r = ldexp(fract, exp);
		node->setState<double>(name, r);
	}
};

template <class T>
struct DASer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		std::ostringstream stream;

		std::vector<T> *r = node->getState<std::vector<T> >(name);
		BOOST_FOREACH(T v, *r) {
			stream << v << " ";
		}
		return stream.str();
	}

	void deserialize(const std::string &invalue,
			 const std::string &name,  Node *node) {
		std::istringstream stream(invalue);
		T v;
		std::vector<T> dv;
		while (!stream.eof()) {
			stream >> v;
			dv.push_back(v);
		}
		std::cout << "deserialized " << dv.size() << " double values" << std::endl;
		node->setState<std::vector<T> >(name, dv);
	}
};

typedef boost::shared_ptr<IStateSerializer> bspss;

type_ser_map IStateSerializer::standard = boost::assign::map_list_of
	(cd3::TypeInfo(typeid(bool)), bspss(new BasicSer<bool>()))
	(cd3::TypeInfo(typeid(int)), bspss(new BasicSer<int>()))
	(cd3::TypeInfo(typeid(long)), bspss(new BasicSer<long>()))
	(cd3::TypeInfo(typeid(double)), bspss(new FPSer()))
	(cd3::TypeInfo(typeid(float)),bspss(new BasicSer<float>()))
	(cd3::TypeInfo(typeid(Flow)), bspss(new FlowSer()))
	(cd3::TypeInfo(typeid(std::string)), bspss(new StringSer()))
	(cd3::TypeInfo(typeid(std::vector<double>)), bspss(new DASer<double>()))
	(cd3::TypeInfo(typeid(std::vector<int>)), bspss(new DASer<int>()));

std::string FlowSerializer::toString(Flow f) {
	std::ostringstream ss;

	BOOST_FOREACH(std::string name, f.getNames()) {
		double r = f.getValue(name);
		int exp;
		double fract = frexp(r, &exp);
		ss << name << " " << fract << " " << exp << " " << cu2string(f.getUnit(name)) << " ";
	}
	return ss.str();
}

Flow FlowSerializer::fromString(const std::string &value) {
	if (value.empty())
		return Flow();
	using namespace boost;
	std::istringstream ss(value);
	Flow f;
	while (!ss.eof()) {
		std::string cname, cunit;
		double fract;
		int exp;
		ss >> cname >> fract >> exp >> cunit;
		Flow::CalculationUnit unit = string2cu(cunit);
		cd3assert(unit != Flow::null, str(format("%2%\nunknown calculation unit: %1%") % cunit % value));
		f.setValue(cname, ldexp(fract, exp));
	}
	return f;
}
