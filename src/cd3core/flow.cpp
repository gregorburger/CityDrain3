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
	FlowDefinition() {
		size = 0;
		defined = false;
	}
	std::vector<std::string> names;
	unordered_map<Flow::CalculationUnit, std::vector<std::string> > unit_names;
	unordered_map<std::string, int> positions;
	unordered_map<std::string, Flow::CalculationUnit > units;
	size_t size;
	bool defined;
};

FlowDefinition Flow::fd;

typedef std::pair<std::string, Flow::CalculationUnit> fdpair;

bool Flow::defined() {
	return fd.defined;
}

void Flow::undefine() {
	if (fd.defined) {
		fd.names.clear();
		fd.positions.clear();
		fd.units.clear();
		fd.unit_names.clear();
		fd.size = 0;
		fd.defined = false;
	}
}

void Flow::define(std::map<std::string, CalculationUnit> definition) {
	cd3assert(!fd.defined, "flow already defined");
	bool qfound = false;
	BOOST_FOREACH(fdpair item, definition) {
		if (item.second != flow)
			continue;
		addUnit(item.first, item.second);
		cd3assert(qfound == false, "flow can only be defined once");
		qfound = true;
	}
	cd3assert(qfound, "at least one flow must be set");
	BOOST_FOREACH(fdpair item, definition) {
		if (item.second == flow)
			continue;
		addUnit(item.first, item.second);
	}

	cd3assert(fd.size == definition.size(), "not all units set");
	fd.defined = true;
}

Flow::Flow() {
	cd3assert(fd.defined, "flow not defined");
	f = shared_array<double>(new double[fd.size]);
	for (size_t i = 0; i < fd.size; i++) {
		f[i] = 0.0;
	}
}

Flow::Flow(const Flow &other) {
	cd3assert(fd.defined, "flow not defined");
	f = other.f;
}

Flow::~Flow() {
}

Flow &Flow::operator =(const Flow &other) {
	cd3assert(fd.defined, "flow not defined");
	f = other.f;
	return *this;
}


void Flow::copyData() {
	cd3assert(fd.defined, "flow not defined");
	if (!f.unique()) {
		double *old = f.get();
		double *narray = new double[fd.size];
		for (size_t i = 0; i < fd.size; i++) {
			narray[i] = old[i];
		}
		f = shared_array<double>(narray);
	}
}

void Flow::addUnit(const std::string &name,
				   const Flow::CalculationUnit unit) {
	cd3assert(unit != Flow::null, "unit is null");
	cd3assert(fd.positions.find(name) == fd.positions.end(), "name already defined");

	fd.names.push_back(name);
	if (fd.unit_names.find(unit) == fd.unit_names.end())
		fd.unit_names[unit] = std::vector<std::string>();
	fd.unit_names[unit].push_back(name);
	fd.positions[name] = fd.size++;
	fd.units[name] = unit;
}

void Flow::setValue(const std::string &name,
					double value) {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(fd.positions.find(name) != fd.positions.end(), "no such name");
	copyData();
	f[fd.positions[name]] = value;
}

double Flow::getValue(const std::string &name) const {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(fd.positions.find(name) != fd.positions.end(), "no such name");
	return f[fd.positions[name]];
}

Flow::CalculationUnit Flow::getUnit(const std::string &name) {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(fd.positions.find(name) != fd.positions.end(), "no such name");
	return fd.units[name];
}

std::vector<std::string> Flow::getNames() {
	cd3assert(fd.defined, "flow not defined");
	return fd.names;
}

std::vector<std::string>
Flow::getUnitNames(Flow::CalculationUnit unit) {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(unit != Flow::null, "null unit not allowed");
	cd3assert(fd.unit_names.find(unit) != fd.unit_names.end(), "no such unit");
	return fd.unit_names[unit];
}

bool Flow::hasName(const std::string &name) {
	cd3assert(fd.defined, "flow not defined");
	return std::count(fd.names.begin(), fd.names.end(), name) > 0;
}

void Flow::dump() const {
	cd3assert(fd.defined, "flow not defined");
	for (size_t i = 0; i < fd.size; i++) {
		std::cout << f[i] << "\t\t";
	}
	std::cout << std::endl;
}

size_t Flow::countUnits(Flow::CalculationUnit unit) {
	cd3assert(fd.defined, "flow not defined");
	return fd.unit_names[unit].size();
}

void Flow::setIth(Flow::CalculationUnit unit, size_t i, double value) {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(fd.unit_names.find(unit) != fd.unit_names.end(), "no such unit");
	cd3assert(fd.unit_names[unit].size() > i, "ith is too much");
	copyData();
	f[fd.positions[fd.unit_names[unit][i]]] = value;
}

double Flow::getIth(Flow::CalculationUnit unit, size_t i) const {
	cd3assert(fd.defined, "flow not defined");
	cd3assert(fd.unit_names.find(unit) != fd.unit_names.end(), "no such unit");
	cd3assert(fd.unit_names[unit].size() > i, "ith is too much");
	return f[fd.positions[fd.unit_names[unit][i]]];
}

void Flow::clear() {
	cd3assert(fd.defined, "flow not defined");
	copyData();
	for (size_t i = 0; i < fd.size; i++) {
		f[i] = 0.0;
	}
}

size_t Flow::size() {
	cd3assert(fd.defined, "flow not defined");
	return fd.size;
}

std::string cu2string(Flow::CalculationUnit c) {
	switch (c) {
		case Flow::flow: return "Flow";
		case Flow::concentration: return "Concentration";
		case Flow::rain: return "Rain";
		case Flow::volume: return "Volume";
		case Flow::climatic: return "Climatic";
		case Flow::temperature: return "Temperature";
		default : return "Null";
	}
}

Flow::CalculationUnit string2cu(std::string s) {
	if (s == "Flow")
		return Flow::flow;

	if (s == "Concentration")
		return Flow::concentration;

	if (s == "Rain")
		return Flow::rain;

	if (s == "Volume")
		return Flow::volume;

	if (s == "Climatic")
		return Flow::climatic;

	if (s == "Temperature")
		return Flow::temperature;

	return Flow::null;
}
