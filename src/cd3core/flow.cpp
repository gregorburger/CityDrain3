//#define MAP_USE_HASH

#include "flow.h"
#ifdef MAP_USE_HASH
#include <boost/unordered/unordered_map.hpp>
#else
#include <map>
#define unordered_map std::map
#endif
#include <algorithm>
#include <boost/foreach.hpp>
#include <cd3assert.h>

using namespace boost;

struct FlowDefinition {
	std::vector<std::string> names;
	unordered_map<Flow::CalculationUnit, std::vector<std::string> > unit_names;
	unordered_map<std::string, int> positions;
	unordered_map<std::string, Flow::CalculationUnit > units;
};

Flow::Flow() {
#ifdef SHARED_FLOW
	f = shared_ptr<FlowPriv>(new FlowPriv());
	fd = shared_ptr<FlowDefinition>(new FlowDefinition());
#else
	f = new FlowPriv();
	fd = new FlowDefinition();
#endif
}

Flow::Flow(const Flow &other) {
#ifdef SHARED_FLOW
	f = other.f;
	fd = other.fd;
#else
	f = new FlowPriv(*other.f);
	fd = new FlowDefinition(*other.fd);
#endif
}

Flow Flow::nullFlow() {
	Flow f;
	f.addUnit("Q", Flow::flow, 0.0);
	return f;
}

Flow::~Flow() {
#ifndef SHARED_FLOW
	delete f;
	delete fd;
#endif
}

Flow &Flow::operator =(const Flow &other) {
#ifdef SHARED_FLOW
	f = other.f;
	fd = other.fd;
#else
	delete f;
	delete fd;
	f = new FlowPriv(*other.f);
	fd = new FlowDefinition(*other.fd);
#endif
	return *this;
}

#ifdef SHARED_FLOW
void Flow::copyData() {
	if (!f.unique()) {
		FlowPriv *old = f.get();
		f = shared_ptr<FlowPriv>
			(new FlowPriv(*old));
	}
}

void Flow::copyDefinition() {
	if (!fd.unique()) {
		FlowDefinition *old = fd.get();
		fd = shared_ptr<FlowDefinition>
			 (new FlowDefinition(*old));
	}
}
#endif

void Flow::addUnit(const std::string &name,
				 const Flow::CalculationUnit unit,
				 double value) {
	cd3assert(unit != Flow::null, "unit is null");
	cd3assert(fd->positions.find(name) == fd->positions.end(), "name already defined");

#ifdef SHARED_FLOW
	copyDefinition();
	copyData();
#endif
	f->push_back(value);
	fd->names.push_back(name);
	if (fd->unit_names.find(unit) == fd->unit_names.end())
		fd->unit_names[unit] = std::vector<std::string>();
	fd->unit_names[unit].push_back(name);
	fd->positions[name] = f->size()-1;
	fd->units[name] = unit;
}

void Flow::setValue(const std::string &name,
					double value) {
	cd3assert(fd->positions.find(name) != fd->positions.end(), "no such name");
#ifdef SHARED_FLOW
	copyData();
#endif
	(*f)[fd->positions[name]] = value;
}

double Flow::getValue(const std::string &name) const {
	cd3assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	return (*f)[fd->positions[name]];
}

Flow::CalculationUnit Flow::getUnit(const std::string &name) const {
	cd3assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	return fd->units[name];
}

const std::vector<std::string> & Flow::getNames() const {
	return fd->names;
}

const std::vector<std::string> &
Flow::getUnitNames(Flow::CalculationUnit unit) const{
	cd3assert(unit != Flow::null, "null unit not allowed");
	cd3assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	return fd->unit_names[unit];
}

bool Flow::hasName(const std::string &name) const {
	return std::count(fd->names.begin(), fd->names.end(), name) > 0;
}

bool Flow::empty() const {
	return fd->names.empty();
}

void Flow::dump() const {
	cout << "name\tvalue\ttype\n";
	BOOST_FOREACH(std::string name, fd->names) {
		std::cout << name << "\t"
				<< getValue(name) << "\t"
				<< cu2string(getUnit(name)) << "\n";
	}
	cout << endl;
}

unsigned int Flow::countUnits(Flow::CalculationUnit unit) const {
	return fd->unit_names[unit].size();
}

void Flow::setIth(Flow::CalculationUnit unit, size_t i, double value) {
	cd3assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	cd3assert(fd->unit_names[unit].size() > i, "ith is too much");
#ifdef SHARED_FLOW
	copyData();
#endif
	(*f)[fd->positions[fd->unit_names[unit][i]]] = value;
}

double Flow::getIth(Flow::CalculationUnit unit, size_t i) const {
	cd3assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	cd3assert(fd->unit_names[unit].size() > i, "ith is too much");
	return (*f)[fd->positions[fd->unit_names[unit][i]]];
}

void Flow::clear() {
#ifdef SHARED_FLOW
	copyData();
#endif
	for (size_t i = 0; i < fd->names.size(); i++) {
		(*f)[i] = 0.0;
	}
}

string cu2string(Flow::CalculationUnit c) {
	switch (c) {
		case Flow::flow: return "Flow";
		case Flow::concentration: return "Concentration";
		case Flow::rain: return "Rain";
		case Flow::volume: return "Volume";
		default : return "Null";
	}
}

Flow::CalculationUnit string2cu(string s) {
	Flow::CalculationUnit unit = Flow::null;
	if (s == "Flow")
		return Flow::flow;

	if (s == "Concentration")
		return Flow::concentration;

	if (s == "Rain")
		return Flow::rain;

	if (s == "Volume")
		return Flow::volume;

	return Flow::null;
}
