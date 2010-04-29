#ifndef PYNODE_H
#define PYNODE_H

#include <Python.h>
#include <boost/python/wrapper.hpp>
#include <node.h>

void wrap_node();

struct NodeWrapper : Node, python::wrapper<Node> {
	NodeWrapper();
	virtual ~NodeWrapper();

	int f(ptime time, int dt);
	bool init(ptime start, ptime stop, int dt);
	void deinit();
	const char *getClassName() const;
	void setClassName(std::string class_name);
	void addInPort(const std::string &name, Flow *inflow);
	void addOutPort(const std::string &name, Flow *outflow);
	void updateParameters();

	void setSelf(boost::python::object self);

	std::map<std::string, int> int_params;
	std::map<std::string, std::string> string_params;
	std::map<std::string, double> double_params;
	std::map<std::string, bool> bool_params;
	std::map<std::string, float> float_params;
	std::map<std::string, std::vector<double> > array_params;
	std::map<std::string, Flow> flow_params;
	char *class_name;
	boost::python::object self;
};

#endif // PYNODE_H
