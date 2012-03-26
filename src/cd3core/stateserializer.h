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

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
using namespace boost;
#include <flow.h>
#include <cd3globals.h>

namespace cd3 {
	class TypeInfo;
}

class Node;

struct IStateSerializer;

typedef std::map<cd3::TypeInfo, boost::shared_ptr<IStateSerializer> > type_ser_map;

struct CD3_PUBLIC IStateSerializer
{
	virtual ~IStateSerializer(){}
	virtual std::string serialize(const std::string &name, Node *node) = 0;
	virtual void deserialize(const std::string &value, const std::string &name,  Node *node) = 0;

	static type_ser_map standard;
};

struct CD3_PUBLIC FlowSerializer {
	static std::string toString(Flow f);
	static Flow fromString(const std::string &s);
};

#endif // SERIALIZER_H
