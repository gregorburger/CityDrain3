#ifndef CALCULATIONUNIT_H
#define CALCULATIONUNIT_H

#include <string>
#include <iostream>
#include <cd3globals.h>

class CD3_PUBLIC ICalculationUnit
{
public:
	//ICalculationUnit(){}
	virtual ~ICalculationUnit(){}
	virtual const std::string &getName() const = 0;
	virtual const std::string &getDescritption() const = 0;
	virtual const std::string &getUnit() const = 0;

	//virtual bool operator == (const ICalculationUnit &other) const = 0;
};

class CD3_PUBLIC CalculationUnit : public ICalculationUnit {
public:
	virtual ~CalculationUnit(){}
	const std::string &getName() const;
	const std::string &getDescritption() const;
	const std::string &getUnit() const;

	static CalculationUnit *null;
	static CalculationUnit *flow;
	static CalculationUnit *volume;
	static CalculationUnit *concentration;
	static CalculationUnit *rain;

	static CalculationUnit *fromString(const std::string &name);

	bool operator == (const ICalculationUnit &other) const;
	bool operator < (const CalculationUnit &other) const;

private:
	CalculationUnit() {
	}
	CalculationUnit(const CalculationUnit &other) {
		(void) other;
	}
	CalculationUnit(const std::string &name, const std::string & description, const std::string &unit);

	std::string name;
	std::string description;
	std::string unit;
};

typedef CalculationUnit CU;

#endif // CALCULATIONUNIT_H
