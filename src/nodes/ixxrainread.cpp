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

#include "ixxrainread.h"
#include "cd3assert.h"
#include "rainbuffer.h"
#include <fstream>
#include <boost/filesystem.hpp>

typedef std::pair<ptime, double> ixx_value;

struct IxxRainReadPrivate {
	std::ifstream in;
	ixx_value parseLine();
	int getDt();
	time_input_facet *in_facet;
	RainBuffer rain_buf;
	ptime current_time;
};

CD3_DECLARE_NODE_NAME(IxxRainRead)

IxxRainRead::IxxRainRead() {
	data = new IxxRainReadPrivate();
	addParameter(ADD_PARAMETERS(rain_file));
	addOutPort(ADD_PARAMETERS(out));
	data->in_facet = new boost::posix_time::time_input_facet();
	data->in_facet->format("%d.%m.%Y %H:%M:%S");
	data->in.imbue(std::locale(data->in.getloc(), data->in_facet));
}

IxxRainRead::~IxxRainRead() {
	delete data->in_facet;
}

bool IxxRainRead::init(ptime start, ptime end, int dt) {
	(void) end;
	(void) dt;
	data->in.close();
	data->in.clear();

	if (!boost::filesystem::exists(rain_file)) {
		Logger(Error) << "can not open rainfile" << rain_file;
		return false;
	}

	data->in.open(rain_file.c_str());
	if (!data->in.is_open()) {
		Logger(Warning) << "it is not possible to read from the given rainfile";
		return false;
	}
	int raindt = data->getDt();
	ptime first_time;
	data->in >> first_time;
	data->current_time = first_time - seconds(raindt);
	data->in.seekg(0, std::ios::beg);

	if (first_time < start) {
		while (data->current_time < start) {
			ixx_value v = data->parseLine();
			time_period p(data->current_time, v.first);
			data->rain_buf.put(p.length().total_seconds(), v.second);
			data->current_time = v.first;
		}

		int cut_off = time_period(first_time, start).length().total_seconds();
		Logger(Debug) << (Node*) this << "cutting of" << cut_off << "after ff rainbuffer";
		data->rain_buf.take(cut_off);
		return true;
	}

	if (first_time > start) { //fill up with zeroes
		long zerosecs = time_period(start, data->current_time + seconds(raindt)).length().total_seconds();
		Logger(Debug) << "filling up rainbuffer with" << zerosecs << "seconds of zero";
		while (zerosecs > 0) {
			data->rain_buf.put(dt, 0.0);
			zerosecs -= dt;
		}
		return true;
	}
	return true;
}

void IxxRainRead::deinit() {
	data->rain_buf.clear();
	data->in.close();
	data->in.clear();
}

int IxxRainRead::f(ptime time, int dt) {
	(void) time;
	while (data->rain_buf.available() < dt) {
		if (data->in.eof()) {
			data->rain_buf.put(dt, 0.0);
			continue;
		}
		ixx_value v = data->parseLine();
		time_period p(data->current_time, v.first);
		data->rain_buf.put(p.length().total_seconds(), v.second);
		data->current_time = v.first;
	}
	out[0] = data->rain_buf.take(dt);
	return dt;
}

ixx_value IxxRainReadPrivate::parseLine() {
	assert(!in.eof());
	ptime date;
	double value;
	in >> date >> value;
	return ixx_value(date, value);
}

int IxxRainReadPrivate::getDt() {
	int pos = in.tellg();
	time_period p(parseLine().first, parseLine().first);
	in.seekg(pos);
	return -p.length().total_seconds();
}
