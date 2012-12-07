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

#ifndef RIVER_H
#define RIVER_H

#include <node.h>
#include <flow.h>
#include <vector>
#include <map>

class Reactor;

/* Combine Sewer and Reactor */

CD3_DECLARE_NODE(River)
public:
	River();
	virtual ~River();

	bool init(ptime start, ptime end, int dt);
	int f(ptime time, int dt);
	void deinit();

private:
	void addMuskParam(int dt);
	void setMuskParam(double *C_x, double *C_y, int dt);

	/* Stuff from Sewer */
	Flow in;
	Flow out;
	int K;
	double X;
	int N;
 //   double latq;
 //   int SED;
	std::vector<Flow*> V;
	std::map<int, std::pair<double, double> > musk_param;

	/* Stuff from ReactorNode */
	std::vector<Reactor*> reactors;
	std::vector<std::string> conc_formula, formula_name;
	std::vector<double> init_v;
	std::string constants;
	int nc, nstep;
};

#endif // SEWER_H
