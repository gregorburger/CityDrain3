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

#ifndef RAINBUFFER_H
#define RAINBUFFER_H

#include <utility>
#include <deque>

typedef std::pair<int, double> rain_event;

class RainBuffer
{
public:
	RainBuffer();

	void put(int dt, double value);
	double take(int dt);
	int available() const;
	void clear();
private:
	int dt;
	std::deque<rain_event> buffer;
	std::deque<int>	avail;
};

#endif // RAINBUFFER_H
