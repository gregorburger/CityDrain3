#ifndef CALCULATIONUNIT_H
#define CALCULATIONUNIT_H

#include <string>
#include <iostream>

class ICalculationUnit
{
public:
	//ICalculationUnit(){}
	//virtual ~ICalculationUnit(){}
	virtual const std::string &getName() const = 0;
	virtual const std::string &getDescritption() const = 0;
	virtual const std::string &getUnit() const = 0;

	//virtual bool operator == (const ICalculationUnit &other) const = 0;
};

class CalculationUnit : public ICalculationUnit {
public:
	//virtual ~CalculationUnit()  {}
	const std::string &getName() const;
	const std::string &getDescritption() const;
	const std::string &getUnit() const;


	/*static CalculationUnit null();
	static CalculationUnit flow();
	static CalculationUnit volume();
	static CalculationUnit concentration();
	*/

	static CalculationUnit *null;
	static CalculationUnit *flow;
	static CalculationUnit *volume;
	static CalculationUnit *concentration;

	/*CalculationUnit &operator=(const CalculationUnit &other) {
		name = other.name;
		description = other.description;
		unit = other.unit;
		return *this;
	}*/

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

//bool operator == (const CalculationUnit &lhs, const CalculationUnit &rhs);
#endif // CALCULATIONUNIT_H
