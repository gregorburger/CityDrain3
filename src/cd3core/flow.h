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

#ifndef FLOW_H
#define FLOW_H

#include <boost/shared_array.hpp>
#include <vector>
#include <cd3globals.h>
#include <string>
#include <map>

struct FlowDefinition;

typedef std::vector<double> FlowPriv;


class CD3_PUBLIC Flow {
public:

	enum CalculationUnit {
		flow,
		concentration,
		rain,
		volume,
		climatic,
		temperature,
		null,
	};

	Flow();
	Flow (const Flow &other);
	virtual ~Flow();

	Flow &operator =(const Flow &rhs);

	void setValue(const std::string &name,
				 double value);

	void clear();

	double getValue(const std::string &name) const;
	static CalculationUnit getUnit(const std::string &name);
	static size_t countUnits(CalculationUnit unit);

	double getIth(CalculationUnit unit, size_t i) const;
	void setIth(CalculationUnit unit, size_t i, double value);

	static std::vector<std::string> getNames();
	static std::vector<std::string> getUnitNames(CalculationUnit unit);
	static bool hasName(const std::string &name);
	static size_t size();

	void dump() const;

	static bool defined();
	static void define(std::map<std::string, CalculationUnit> definition);
	static void undefine();

	inline double &operator[](int i) {
		copyData();
		return f[i];
	}

	inline const double &operator[](int i) const {
		return f[i];
	}

private:
	static void addUnit(const std::string &name, CalculationUnit unit);
	static FlowDefinition fd;
	boost::shared_array<double> f;
	void copyData();
};

CD3_PUBLIC std::string cu2string(Flow::CalculationUnit c);
CD3_PUBLIC Flow::CalculationUnit string2cu(std::string s);


#endif // FLOW_H
