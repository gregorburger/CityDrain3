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

#ifndef FLOWFUNS_H
#define FLOWFUNS_H

#include <utility>
#include <vector>
#include <string>
#include <cd3globals.h>

class Flow;

struct CD3_PUBLIC FlowFuns {

static Flow mix(const std::vector<Flow> &inputs);
static Flow mix(const std::vector<Flow*> &inputs);

static std::pair<Flow, Flow> split(const Flow &f,
								   float ratio);

static Flow catchment_flowmodel(const Flow &f,
								double area,
								int dt,
								const std::vector<double> &cvalues);

static Flow catchement_lossmodel(const Flow &f,
								 Flow *basin,
								 double init_loss,
								 double perma_loss,
								 double rf_coeff);

static Flow route_catchment(const Flow &inflow,
							Flow rain,
							Flow *volume,
							int N,
							double C_x,
							double C_y,
							int dt);

static Flow route_sewer(const Flow &inflow,
						Flow *volume,
						double C_x,
						double C_y,
						int dt);
};
#endif // FLOWFUNS_H
