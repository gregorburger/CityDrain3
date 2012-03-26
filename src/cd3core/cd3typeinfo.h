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

#ifndef TYPEID_H
#define TYPEID_H

#include <string.h>
#include <stdlib.h>
#include <typeinfo>
#include <cd3globals.h>
#include <string>

namespace cd3 {

class TypeInfo {
public:
	TypeInfo();
	TypeInfo(const std::type_info &info);
	//TypeInfo(const TypeInfo &other);
	//~TypeInfo();
	std::string type_name;
};


inline bool operator==(const TypeInfo &lhs, const TypeInfo &rhs) {
	return lhs.type_name.compare(rhs.type_name.c_str()) == 0;
	return true;
}

inline bool operator!=(const TypeInfo &lhs, const TypeInfo &rhs) {
	return !(rhs == lhs);
	return true;
}

inline bool operator < (const TypeInfo &lhs, const TypeInfo &rhs) {
	return lhs.type_name.compare(rhs.type_name.c_str()) < 0;
}

inline TypeInfo::TypeInfo() {
	class Nil{};
	type_name = typeid(Nil).name();
}

inline TypeInfo::TypeInfo(const std::type_info &info) {
	type_name = info.name();
}

}
#endif // TYPEID_H
