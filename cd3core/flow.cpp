#include "flow.h"
#include <boost/unordered/unordered_map.hpp>
#include <calculationunit.h>
#include <boost/foreach.hpp>
#include <cd3assert.h>

using namespace boost;

struct FlowDefinition {
	std::vector<std::string> names;
        unordered_map<const CalculationUnit *, std::vector<std::string> > unit_names;
        unordered_map<std::string, int> positions;
        unordered_map<std::string, const CalculationUnit *> units;
};

Flow::Flow() {
#ifdef SHARED_FLOW
	f = boost::shared_ptr<FlowPriv>(new FlowPriv());
	fd = boost::shared_ptr<FlowDefinition>(new FlowDefinition());
#else
	//std::cout << "cons " << this << std::endl;
	f = new FlowPriv();
	fd = new FlowDefinition();
#endif
}

Flow::Flow(const Flow &other) {
#ifdef SHARED_FLOW
	f = other.f;
	fd = other.fd;
#else
	//std::cout << "copy " << this << std::endl;
	f = new FlowPriv(*other.f);
	fd = new FlowDefinition(*other.fd);

	/**f = *other.f;
	*fd = *other.fd;*/
#endif
}

Flow Flow::nullFlow() {
	Flow f;
	f.addUnit("flow", CU::flow, 0.0);
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
	//std::cout << "assign " << this << std::endl;
	delete f;
	delete fd;
	f = new FlowPriv(*other.f);
	fd = new FlowDefinition(*other.fd);
/*	*f = *other.f;
	*fd = *other.fd;*/
#endif
	return *this;
}

/*void Flow::copy() {
	copyData();
	copyDefinition();
}*/

#ifdef SHARED_FLOW
void Flow::copyData() {
	if (!f.unique()) {
		FlowPriv *old = f.get();
		f = boost::shared_ptr<FlowPriv>(new FlowPriv(*old));
	}
}

void Flow::copyDefinition() {
	if (!fd.unique()) {
		FlowDefinition *old = fd.get();
		fd = boost::shared_ptr<FlowDefinition>(new FlowDefinition(*old));
	}
}
#endif

void Flow::addUnit(const std::string &name,
				 const CalculationUnit *unit,
				 double value) {
	cd3assert(fd->positions.find(name) == fd->positions.end(), "name already defined");
	cd3assert(unit, "unit is null");

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

const CalculationUnit *Flow::getUnit(const std::string &name) const {
	cd3assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	return fd->units[name];
}

const std::vector<std::string> & Flow::getNames() const {
	return fd->names;
}

const std::vector<std::string> &
Flow::getUnitNames(const CalculationUnit *unit) const{
	cd3assert(unit, "null unit not allowed");
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
	BOOST_FOREACH(std::string name, fd->names) {
		std::cout << "flow.dump.names " << name << std::endl;
	}
}

unsigned int Flow::countUnits(const CalculationUnit *unit) const {
	return fd->unit_names[unit].size();
}

void Flow::setIth(const CalculationUnit *unit, size_t i, double value) {
	cd3assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	cd3assert(fd->unit_names[unit].size() > i, "ith is too much");
#ifdef SHARED_FLOW
	copyData();
#endif
	(*f)[fd->positions[fd->unit_names[unit][i]]] = value;
}

double Flow::getIth(const CalculationUnit *unit, size_t i) const {
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
