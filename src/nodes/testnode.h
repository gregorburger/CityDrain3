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

#ifndef TESTNODE_H
#define TESTNODE_H

#include <node.h>

class Flow;

CD3_DECLARE_NODE(TestNode)
public:
	TestNode();
	virtual ~TestNode();
	int f(ptime time, int dt);

private:
	Flow *in_out;
	Flow *test;
	int int_value;
	double double_value;
	std::string string_value;
};

#endif // TESTNODE_H
