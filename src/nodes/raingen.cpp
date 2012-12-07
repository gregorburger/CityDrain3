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

#include "raingen.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <time.h>
#include <math.h>

using namespace boost;

#include <cd3assert.h>



inline double mattimax(double a, double b){return ((a>b)?a:b);}
inline double mattimin(double a, double b){return ((a>b)?b:a);}

CD3_DECLARE_NODE_NAME(RainGen)


double RainGen::getNormRandNumbers(){
	if(this->norm_rand_nr_is_good){
		this->norm_rand_nr_is_good=false;
		return this->norm_rand_nr;
	}

	double s=0;
	double v1;
	double v2;

	while(s>1 || s<=0){
		v1 = double(rand())/double(RAND_MAX);
		v2 = double(rand())/double(RAND_MAX);

		v1 = v1*2.0-1.0;
		v2 = v2*2.0-1.0;

		s = v1*v1+v2*v2;
	}

	double x1 = v1*sqrt(-2.0*log(s)/s);
	double x2 = v2*sqrt(-2.0*log(s)/s);

	this->norm_rand_nr=x2;
	this->norm_rand_nr_is_good=true;

	return x1;
}

RainGen::RainGen() {

	addOutPort(ADD_PARAMETERS(out));

	a = 7;
	b = 70;
	c = 1000;

	addParameter(ADD_PARAMETERS(a)).setUnit("1/day");
	addParameter(ADD_PARAMETERS(b)).setUnit("1/day");
	addParameter(ADD_PARAMETERS(c)).setUnit("sec/mm/sec");
	addParameter(ADD_PARAMETERS(d));

	addState(ADD_PARAMETERS(t_rain));
	addState(ADD_PARAMETERS(t_dry));
	addState(ADD_PARAMETERS(i_rain));
	addState(ADD_PARAMETERS(i_rain_laststep));

}

RainGen::~RainGen() {
}

bool RainGen::init(ptime start, ptime end, int dt) {
	(void) dt;
	(void) start;


	srand(time(NULL));

	t_rain=-1;
	t_dry=-1;
	i_rain=-1;

	norm_rand_nr_is_good=false;

	return true;
}

int RainGen::f(ptime time, int dt) {
	(void) time;

	double t_rain_step;
	if(t_rain<=0){
		t_dry=-log(double(rand())/double(RAND_MAX))/a*86400;
		t_rain=-log(double(rand())/double(RAND_MAX))/b*86400;
		i_rain=-log(double(rand())/double(RAND_MAX))/c/t_rain;
		i_rain_laststep = 0;

	}
	if((t_dry-dt)<0){
		t_rain_step = mattimin(dt-t_dry, t_rain);
		t_dry=0;
		t_rain-=t_rain_step;

		double x = getNormRandNumbers();
		out[0]=i_rain_laststep=mattimax(0.0,(i_rain_laststep/2.0+x*t_rain_step*d*i_rain));
	}
	else{
		t_dry-=dt;
		out[0]=0;
	}


	return dt;
}
