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

#include "testnode.h"
#include <flow.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

CD3_DECLARE_NODE_NAME(TestNode)

TestNode::TestNode() {
	in_out = new Flow();
	test = new Flow();
	addOutPort("out", in_out);
	addInPort("in", in_out);
	int_value = 10;
	double_value = 3.142;
	string_value = "los gehts";
	addState(ADD_PARAMETERS(int_value));
	addState(ADD_PARAMETERS(double_value));
	addState(ADD_PARAMETERS(string_value));
	addState(ADD_PARAMETERS_P(test));
}

TestNode::~TestNode() {
	delete in_out;
}

int TestNode::f(ptime time, int dt) {
	int_value += dt;
	double_value += dt;
	string_value += str(boost::format(" hallo %1% ") % time);

	test->dump();
	*test = Flow();
	test->dump();

	return dt;
}
