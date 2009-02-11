#ifndef FLOW_H
#define FLOW_H

#include <map>
#include <iostream>
#include <limits>
#include <boost/shared_ptr.hpp>
#include "calculationunit.h"

template<typename T>
class SafeArrayAccess;

typedef std::multimap<std::string, CalculationUnit> fdt;
typedef std::pair<std::string, CalculationUnit> fdtp;
typedef std::map<std::string, int> simap; //(s)tring (i)nt map

template<typename T>
class SafeArrayAccess {
public:
	SafeArrayAccess(T *data, int size, int offset);

	T &operator[](const int index);

private:
	T *data;
	int size;
	int offset;
	T nan;
};

typedef std::map<CalculationUnit *, int> cu_to_int;
typedef std::multimap<CalculationUnit *, std::string> cu_mto_str;
typedef std::pair<CalculationUnit *, std::string> pair_units;
typedef std::map<std::string, int> str_to_int;

struct FlowDefinition {
	void addUnit(const std::string &, CalculationUnit *);

	cu_to_int offsets;
	str_to_int indices;
	cu_mto_str units;
};

class Flow
{
public:
	Flow();
	~Flow();
	Flow(const Flow &other);
	Flow &operator=(const Flow &other);


	Flow *addUnit(const std::string &name, CalculationUnit *unit, double value);

	int getUnitCount(CalculationUnit &unit) const;
	SafeArrayAccess<double> getUnitData(CalculationUnit &unit);
	double getValue(const std::string &name);

	void dump();

private:
	void initData();

private:
	bool shared;
	bool locked;


	double *data;
	boost::shared_ptr<FlowDefinition> fd;
	std::map<std::string, double> *tmp_values;
};

template<typename T>
SafeArrayAccess<T>::SafeArrayAccess(T *data, int size, int offset)
: data(data), size(size), offset(offset) {
	nan = std::numeric_limits<T>::quiet_NaN();
}

template<typename T>
T &SafeArrayAccess<T>::operator [](const int index) {
	if (index < size) {
		return data[offset+index];
	} else {
		std::cerr << "Warning: array out of bounds" << std::endl;
	}
	return nan;
}

#endif // FLOW_H
