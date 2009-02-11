#include "flow.h"
#include <iostream>
#include <cstring>

void FlowDefinition::addUnit(const std::string &name, CalculationUnit *unit) {
	units.insert(pair_units(unit, name));
}

Flow::Flow()
	: shared(false), data(0), fd(new FlowDefinition()) {
	tmp_values = new std::map<std::string, double>();
}

Flow::Flow(const Flow &other)
		: shared(true), data(0), fd(other.fd), tmp_values(0) {

}

Flow &Flow::operator =(const Flow &other) {
	fd = other.fd;
	data = 0;
	//data = new double[fd->units.size()];
	//std::memcpy(data, other.data, sizeof(double) * fd->units.size());
	return *this;
}

Flow::~Flow() {
	if (data) {
		delete data;
	}

	if (!shared) {
		delete tmp_values;
	}
}

Flow *Flow::addUnit(const std::string &name,
					CalculationUnit *unit,
					double value) {

	if (data != 0) {
		std::cerr << "flow values lost" << std::endl;
	}
	(*tmp_values)[name] = value;
	fd->addUnit(name, unit);
	return this;
}


void Flow::initData() {
	if (data) {
		std::cerr << "WARNING: flow values lost" << std::endl;
		delete data;
	}

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
		const double value = (*tmp_values)[name];
		data[i] = value;
		i++;
	}
	delete tmp_values;
	locked = true;
}

SafeArrayAccess<double> Flow::getUnitData(CalculationUnit &unit) {
	if (!data) {
		initData();
	}
	return SafeArrayAccess<double>(data, fd->units.count(&unit), fd->offsets[&unit]);
}

double Flow::getValue(const std::string &name) {
	if (!data) {
		initData();
	}
	int index = fd->indices[name];
	return data[index];
}

void Flow::dump() {
	if (!data) {
		initData();
	}
	std::cout << fd->units.size() << " values stored" << std::endl;
	for (cu_mto_str::size_type i = 0; i < fd->units.size(); i++) {
		std::cout << "data[" << i << "]=" << data[i] << std::endl;
	}
}
