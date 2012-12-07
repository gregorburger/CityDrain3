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

#ifndef TYPEFACTORY_H
#define TYPEFACTORY_H

#include <memory>
#include <string>
#include <QDomElement>
#include <boost/shared_ptr.hpp>
#include <cd3globals.h>

class Node;

class CD3_PUBLIC ITypeFactory {
public:
	virtual std::string getTypeName() = 0;

	virtual void setParameter(Node *n,
							  const std::string &parameter,
							  QDomElement &element) = 0;

	virtual void setState(Node *n,
						  const std::string &parameter,
						  QDomElement &element) = 0;
};


#define CD3_DECLARE_TYPE_FACTORY(Type) \
class CD3_PUBLIC Type##TypeFactory : public ITypeFactory { \
public: \
	std::string getTypeName() { return #Type; } \
private:


#endif // TYPEFACTORY_H
