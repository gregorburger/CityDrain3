#include "stateserializer.h"
#include "node.h"
#include "flow.h"
#include "calculationunit.h"
#include "cd3typeinfo.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <sstream>

#include <cd3assert.h>

struct BoolSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, bool>(*node->getState<bool>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		bool ivalue = boost::lexical_cast<bool>(value);
		node->setState<bool>(name, ivalue);
	}
};

struct IntSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, int>(*node->getState<int>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		int ivalue = boost::lexical_cast<int>(value);
		node->setState<int>(name, ivalue);
	}
};

struct LongSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, long>(*node->getState<long>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		long ivalue = boost::lexical_cast<long>(value);
		node->setState<long>(name, ivalue);
	}
};

struct FloatSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, float>(*node->getState<float>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		float fvalue = boost::lexical_cast<float>(value);
		node->setState<float>(name, fvalue);
	}
};

struct DoubleSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return boost::lexical_cast<std::string, double>(*node->getState<double>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
		double dvalue = boost::lexical_cast<double, std::string>(value);
		node->setState<double>(name, dvalue);
	}
};

struct FlowSer : public IStateSerializer {
	std::string serialize(const std::string &name, Node *node) {
		return FlowSerializer::toString(*node->getState<Flow>(name));
	}

	void deserialize(const std::string &value,
					 const std::string &name,  Node *node) {
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

type_ser_map IStateSerializer::standard() {
	type_ser_map s;
	s[cd3::TypeInfo(typeid(bool))] = boost::shared_ptr<IStateSerializer>(new BoolSer());
	s[cd3::TypeInfo(typeid(int))] = boost::shared_ptr<IStateSerializer>(new IntSer());
	s[cd3::TypeInfo(typeid(long))] = boost::shared_ptr<IStateSerializer>(new LongSer());
	s[cd3::TypeInfo(typeid(double))] = boost::shared_ptr<IStateSerializer>(new DoubleSer());
	s[cd3::TypeInfo(typeid(float))] =boost::shared_ptr<IStateSerializer>( new FloatSer());
	s[cd3::TypeInfo(typeid(Flow))] = boost::shared_ptr<IStateSerializer>(new FlowSer());
	s[cd3::TypeInfo(typeid(std::string))] = boost::shared_ptr<IStateSerializer>(new StringSer());
	return s;
}


std::string FlowSerializer::toString(Flow f) {
	std::ostringstream ss;

	BOOST_FOREACH(std::string name, f.getNames()) {
		ss << name << " " << f.getValue(name) << " " << f.getUnit(name)->getName() << " ";
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
		std::string cname, cvalue, cunit;
		ss >> cname >> cvalue >> cunit;
		CalculationUnit *unit = CalculationUnit::fromString(cunit);
		assert(unit != CalculationUnit::null, str(format("%2%\nunknown calculation unit: %1%") % cunit % value));
		f.addUnit(cname, unit, boost::lexical_cast<double>(cvalue));
	}
	return f;
}
