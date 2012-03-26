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

#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <string>
#include <typeinfo>

#include "node.h"
#include "cd3utils.h"
#include <boost/utility.hpp>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>

class CD3_PUBLIC INodeFactory {
public:
	virtual ~INodeFactory(){}
	virtual Node *createNode() const = 0;
	virtual std::string getNodeName() const = 0;
	virtual std::string getSource() const = 0;
};

template <typename T>
class CD3_PUBLIC NodeFactory
	: public INodeFactory {
public:
	NodeFactory(std::string source);
	virtual Node *createNode() const;
	virtual std::string getNodeName() const;
	virtual std::string getSource() const;
private:
	std::string source;
};

template <typename T>
NodeFactory<T>::NodeFactory(std::string source) : source(source) {
}

template <typename T>
Node *NodeFactory<T>::createNode() const {
	return new T();
}

template <typename T>
std::string NodeFactory<T>::getNodeName() const {
	return T::class_name;
}

template <typename T>
std::string NodeFactory<T>::getSource() const {
	return source;
}

#endif // NODEFACTORY_H
