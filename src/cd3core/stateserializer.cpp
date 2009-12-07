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

typedef boost::shared_ptr<IStateSerializer> bspss;

type_ser_map IStateSerializer::standard = boost::assign::map_list_of
	(cd3::TypeInfo(typeid(bool)), bspss(new BasicSer<bool>()))
	(cd3::TypeInfo(typeid(int)), bspss(new BasicSer<int>()))
	(cd3::TypeInfo(typeid(long)), bspss(new BasicSer<long>()))
	(cd3::TypeInfo(typeid(double)), bspss(new FPSer()))
	(cd3::TypeInfo(typeid(float)),bspss(new BasicSer<float>()))
	(cd3::TypeInfo(typeid(Flow)), bspss(new FlowSer()))
	(cd3::TypeInfo(typeid(std::string)), bspss(new StringSer()));

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
