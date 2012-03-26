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

#ifndef RAINGEN_H
#define RAINGEN_H

#include <node.h>
#include <flow.h>


CD3_DECLARE_NODE(RainGen)
public:
	RainGen();
	virtual ~RainGen();
	int f(ptime time, int dt);
	bool init(ptime start, ptime end, int dt);

private:
	//polar-method, see Knuth books
	//return the first, save the second
	//or return the second
	double getNormRandNumbers();
	double a,b,c,d;
	double t_rain, t_dry;
	double i_rain;
	double i_rain_laststep;
	Flow out;
	double norm_rand_nr;
	bool norm_rand_nr_is_good;
};

#endif // RAINGEN_H
