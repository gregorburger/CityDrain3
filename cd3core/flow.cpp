#include "flow.h"
#include <map>
#include <calculationunit.h>
#include <boost/foreach.hpp>
#include <cd3assert.h>

struct FlowDefinition {
	std::vector<std::string> names;
	std::map<const CalculationUnit *, std::vector<std::string> > unit_names;
	std::map<std::string, int> positions;
};

Flow::Flow() {
	f = boost::shared_ptr<FlowPriv>(new FlowPriv());
	fd = boost::shared_ptr<FlowDefinition>(new FlowDefinition());
}

Flow::Flow(const Flow &other) {
	f = other.f;
	fd = other.fd;
}

Flow::~Flow() {

}

Flow &Flow::operator =(const Flow &other) {
	f = other.f;
	fd = other.fd;
	return *this;
}

void Flow::copy() {
	copyData();
	copyDefinition();
}

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

void Flow::addUnit(const std::string &name,
				 const CalculationUnit *unit,
				 double value) {
	assert(fd->positions.find(name) == fd->positions.end(), "name already defined");
	assert(unit, "unit is null");

	copyDefinition();
	copyData();
	f->push_back(value);
	fd->names.push_back(name);
	if (fd->unit_names.find(unit) == fd->unit_names.end())
		fd->unit_names[unit] = std::vector<std::string>();
	fd->unit_names[unit].push_back(name);
	fd->positions[name] = f->size()-1;
}

void Flow::setValue(const std::string &name,
					double value) {
	assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	copyData();
	(*f)[fd->positions[name]] = value;
}

double Flow::getValue(const std::string &name) const {
	assert(fd->positions.find(name) != fd->positions.end(), "no such name");
	return (*f)[fd->positions[name]];
}

const std::vector<std::string> & Flow::getNames() const {
	return fd->names;
}

const std::vector<std::string> &
Flow::getUnitNames(const CalculationUnit *unit) const{
	assert(unit, "null unit not allowed");
	assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	return fd->unit_names[unit];
}

void Flow::dump() const {
	BOOST_FOREACH(std::string name, fd->names) {
		std::cout << "flow.dump.names " << name << std::endl;
	}
}


void mix(Flow *f, const Flow * const *inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->setValue(qename, qe);
	//f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		//f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
		f->setValue(cname, Ci/qe);
	}
}

Flow *mix(const Flow * const *inputs, int num_inputs) {
	assert(num_inputs > 1, "cannot mix one input");
	double qe = 0;
	std::string qename = inputs[0]->getUnitNames(CalculationUnit::flow)[0];

	Flow *f = new Flow();

	for (int i = 0; i < num_inputs; i++) {
		assert(inputs[0]->getUnitNames(CalculationUnit::flow).size() == 1,
			   "unable to mix more than one flow");
		qe += inputs[i]->getValue(qename);
	}

	f->addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f->addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}


Flow mix(const std::vector<const Flow *> &inputs) {
	int num_inputs = inputs.size();
	assert(num_inputs > 1, "cannot mix one input");

	Flow f0 = *inputs[0];
	int flowsize = f0.getUnitNames(CalculationUnit::flow).size();
	assert(flowsize == 1, "unable to mix more than one flow");
	std::string qename = f0.getUnitNames(CalculationUnit::flow)[0];

	Flow f;
	double qe = 0;


	for (int i = 0; i < num_inputs; i++) {
		qe += inputs[i]->getValue(qename);
	}

	f.addUnit(qename, CalculationUnit::flow, qe);

	for (size_t c = 0;
		 c < inputs[0]->getUnitNames(CalculationUnit::concentration).size();
		 c++) {
		double Ci = 0;
		std::string cname = inputs[0]->getUnitNames(CalculationUnit::concentration).at(c);
		for (int i = 0; i < num_inputs; i++) {
			Ci += inputs[i]->getValue(cname);
		}
		f.addUnit(cname, CalculationUnit::concentration, Ci/qe);
	}
	return f;
}

std::pair<Flow, Flow> split(const Flow flow, float ratio) {
	assert(ratio <= 1.0 && ratio >= 0.0, "ratio must me between 0 and 1");
	Flow f1 = flow;
	Flow f2 = flow;

	std::string qename = flow.getUnitNames(CalculationUnit::flow)[0];
	f1.setValue(qename, flow.getValue(qename) * ratio);
	f2.setValue(qename, flow.getValue(qename) * (1 - ratio));

	return std::pair<Flow, Flow>(f1, f2);
}

double Flow::getIth(const CalculationUnit *unit, size_t i) const {
	assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	assert(fd->unit_names[unit].size() > i, "ith is too much");
	return (*f)[fd->positions[fd->unit_names[unit][i]]];
}

void Flow::setIth(const CalculationUnit *unit, size_t i, double value) {
	assert(fd->unit_names.find(unit) != fd->unit_names.end(), "no such unit");
	assert(fd->unit_names[unit].size() > i, "ith is too much");
	copyData();
	(*f)[fd->positions[fd->unit_names[unit][i]]] = value;
}

void Flow::clear() {
	copyData();
	for (size_t i = 0; i < fd->names.size(); i++) {
		(*f)[i] = 0.0;
	}
}
