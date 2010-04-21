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
	void setClassName(string class_name);
	void addInPort(const std::string &name, Flow *inflow);
	void addOutPort(const std::string &name, Flow *outflow);
	void updateParameters();

	void setSelf(boost::python::object self);

	map<string, int> int_params;
	map<string, string> string_params;
	map<string, double> double_params;
	map<string, bool> bool_params;
	map<string, float> float_params;
	map<string, vector<double> > array_params;
	map<string, Flow> flow_params;
	char *class_name;
	boost::python::object self;
};

#endif // PYNODE_H
