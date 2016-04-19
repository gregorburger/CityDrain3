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
    try
    {
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
        int dataoffset = time_period(data->current_time, start - seconds(dt)).length().total_seconds();

        Logger(Debug) << "First time stamp of rain data: " << to_simple_string(first_time);
        Logger(Debug) << "dt of rain data (seconds): " << to_simple_string(seconds(raindt));
        Logger(Debug) << "Rain measurements started at: " << to_simple_string(data->current_time);
        Logger(Debug) << "Simulation first time step: " << to_simple_string(start);
        Logger(Debug) << "Simulation start time: " << to_simple_string(start - seconds(dt));
        Logger(Debug) << "Rain data starts " << dataoffset << " seconds earlier than simulation.";

        if (dataoffset >= 0) {
            while (time_period(data->current_time, start - seconds(dt)).length().total_seconds() >= 0) {
                if(data->in.eof())
                {
                    Logger(Error) << "Simulation starts after rain data";
                    return false;
                }
                ixx_value v = data->parseLine();
                data->rain_buf.put(raindt, v.second);
                data->current_time = v.first;
            }

            Logger(Debug) << (Node*) this << "Cutting of" << dataoffset << " seconds of raindata";
            data->rain_buf.take(dataoffset);
            return true;
        }
        else{
            Logger(Debug) << "filling up rainbuffer with" << dataoffset*-1 << "seconds of zero";
            data->rain_buf.put(dataoffset*(-1), 0.0);
            return true;
        }

        return false;
    }
    catch (char const* m)
    {
        Logger(Error) << this << m;
        return false;
    }
}

void IxxRainRead::deinit() {
	data->rain_buf.clear();
	data->in.close();
	data->in.clear();
}

int IxxRainRead::f(ptime time, int dt) {
    try
    {
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
    catch (char const* m)
    {
        Logger(Error) << this << m;
        return 0.0;
    }
}

ixx_value IxxRainReadPrivate::parseLine() {
	assert(!in.eof());
    ptime date;
    double value;

    in >> date;

    if(date == boost::posix_time::not_a_date_time && !in.eof())
        throw "Data_time format not valid. Format shut be: dd.mm.yyyy hh:mm:ss";

    in >> value;
	return ixx_value(date, value);
}

int IxxRainReadPrivate::getDt() {
	int pos = in.tellg();
	time_period p(parseLine().first, parseLine().first);
	in.seekg(pos);
	return -p.length().total_seconds();
}
