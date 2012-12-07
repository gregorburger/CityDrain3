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

#ifndef REACT_H
#define REACT_H

#include <string>
#include <vector>
#include <flow.h>

namespace mu {
class Parser;
}

class Reactor
{
public:
	Reactor(Flow &volume, bool use_outer_volume = false);
	virtual ~Reactor();
	bool init(int dt,
			  std::string constants,
			  std::vector<std::string> conc_formula);
	void react(Flow &in, int dt);
	void react_dV(Flow &in, double d_reactor_volume, int dt);
	void react_dV_SED(Flow &in, double d_reactor_volume, int SED, int dt);

	std::vector<double> init_v;
	double reactor_volume;
	int nstep;
private:
	Flow &volume;
	double acc;
	std::vector<mu::Parser *> parsers;
	bool use_outer_volume;
};

#endif // REACT_H
