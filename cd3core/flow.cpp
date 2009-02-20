#include "flow.h"
#include <iostream>
#include <cstring>
#include <cd3assert.h>

Flow::Flow()
	: data(0), fd(new FlowDefinition()), prepared(false) {
}

Flow::Flow(const Flow &other)
 :data(0) {
	fd = other.fd;
	assert(other.prepared, "assignement of flow: other flow data not initialized");
	data = new double[getSize()];
	memcpy(data, other.data, sizeof(double) * getSize());
	prepared = true;
}

Flow &Flow::operator =(const Flow &other) {
	fd = other.fd;
	if (prepared)
		delete[] data;
	assert(other.prepared, "assignement of flow: other flow data not initialized");
	data = new double[getSize()];
	memcpy(data, other.data, sizeof(double) * getSize());
	prepared = true;
	return *this;

}

Flow::~Flow() {
	if (prepared) {
		delete[] data;
	}
}

Flow *Flow::addUnit(const std::string &name,
					CalculationUnit *unit,
					double value) {
	assert(!prepared, "flow data already fixed");
	tmp_values[name] = value;
	fd->addUnit(name, unit);
	return this;
}

const std::vector<std::string> &Flow::getNames() {
	return fd->names;
}

void Flow::prepareData() {
	assert(!prepared, "flow data already fixed");
	data = new double[fd->units.size()];
	int i = 0;
	CalculationUnit *tmp = NULL;

	for (cu_mto_str::iterator it = fd->units.begin(); it != fd->units.end(); it++) {
		if (tmp != it->first) {
			fd->offsets[it->first] = i;
			tmp = it->first;
		}
		const std::string name = it->second;
		fd->indices[name] = i;
		const double value = tmp_values[name];
		data[i] = value;
		i++;
	}
	prepared = true;
}

void FlowDefinition::addUnit(const std::string &name, CalculationUnit *unit) {
	units.insert(pair_units(unit, name));
	names.push_back(name);
}

int Flow::getSize() const {
	return fd->names.size();
}

SafeArrayAccess<double> Flow::getUnitData(CalculationUnit &unit) {
	assert(prepared, "flow data not prepared");
	return SafeArrayAccess<double>(data, fd->units.count(&unit), fd->offsets[&unit]);
}

double Flow::getValue(const std::string &name) {
	assert(prepared, "flow data not prepared");
	int index = fd->indices[name];
	return data[index];
}

const double *Flow::getData() const {
	assert(prepared, "flow data not prepared");
	return data;
}

int Flow::getUnitCount(CalculationUnit &unit) const {
	return fd->units.count(&unit);
}

std::vector<std::string> Flow::getUnitNames(CalculationUnit &unit) const {
	std::vector<std::string> names;

	cu_mto_str::const_iterator it = fd->units.find(&unit);

	while (it != fd->units.end()) {
		names.push_back(it->second);
	}

	return names;
}

void Flow::dump() {
	assert(prepared, "flow data not prepared");
	std::cout << fd->units.size() << " values stored" << std::endl;
	for (cu_mto_str::size_type i = 0; i < fd->units.size(); i++) {
		std::cout << "data[" << i << "]=" << data[i] << std::endl;
	}
}
