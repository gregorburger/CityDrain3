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

#ifndef NODEREGISTRY_H
#define NODEREGISTRY_H

#include <map>
#include <string>
#include <vector>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>
#include <flowdefinition.h>
using namespace boost;

class INodeFactory;
class Node;

#ifndef PYTHON_DISABLED
#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#include <Python.h>
#endif

typedef std::map<std::string, INodeFactory *> reg_node_type;

class CD3_PUBLIC NodeRegistry
{
public:
	NodeRegistry();
	~NodeRegistry();

	bool addNodeFactory(INodeFactory *factory);
	void addNativePlugin(const std::string &plugin_path);

#ifndef PYTHON_DISABLED
	void addPythonPlugin(const std::string &script);
	static void addToPythonPath(std::string p);
#endif

	std::vector<std::string> getRegisteredNames() const;

	std::map<std::string, std::vector<std::string> >
	getRegisteredNamesAndSources() const;

	Node *createNode(const std::string &name) const;
	bool contains(const std::string &name) const;

private:
	reg_node_type registered_nodes;
#ifndef PYTHON_DISABLED
	static std::vector<std::string> python_paths;
	static PyObject *main_namespace;
#endif
};

typedef void (*regNodeFunProto) (NodeRegistry *reg);

#endif // NODEREGISTRY_H
