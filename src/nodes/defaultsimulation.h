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

#ifndef DEFAULTSIMULATION_H
#define DEFAULTSIMULATION_H

#include <simulation.h>
#include <imodel.h>

struct SimPriv;

class CD3_PUBLIC DefaultSimulation : public ISimulation {
public:
	DefaultSimulation();
	virtual ~DefaultSimulation();

	virtual void start(ptime time);
	int run(ptime time, int dt);
	void setModel(IModel *model);

private:
	void run(Node *n, ptime time, con_count_type &deps);

private: //data member
	SimPriv *sp;
};

#endif // DEFAULTSIMULATION_H
