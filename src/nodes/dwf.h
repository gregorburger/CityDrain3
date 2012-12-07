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

#ifndef DWF_H
#define DWF_H

#include <node.h>
#include <flow.h>
#include <deque>

typedef std::pair<int, std::vector<double> > dwf_entry;

CD3_DECLARE_NODE(DWF)
public:
	DWF();
	virtual ~DWF();
	bool init(ptime start, ptime end, int dt);
	void deinit();
	int f(ptime time, int dt);
private:
	bool load(int dt);
	void setOut();
	std::string dwf_definition_file;
	double q_scale;
	Flow out;
	std::deque<dwf_entry> dwf, dwf_work;
	int mdt;
};

#endif // DWF_H
