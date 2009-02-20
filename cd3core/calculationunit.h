#ifndef CALCULATIONUNIT_H
#define CALCULATIONUNIT_H

#include <string>

class ICalculationUnit
{
public:
	ICalculationUnit(){}
	virtual ~ICalculationUnit(){}
	virtual const std::string &getName() const = 0;
	virtual const std::string &getDescritption() const = 0;
	virtual const std::string &getUnit() const = 0;

	virtual bool operator == (const ICalculationUnit &other) = 0;
};

class CalculationUnit : public ICalculationUnit {
public:
	CalculationUnit() {}
	~CalculationUnit()  {}
	const std::string &getName() const;
	const std::string &getDescritption() const;
	const std::string &getUnit() const;

	static CalculationUnit flow;
	static CalculationUnit volume;
	static CalculationUnit concentration;

	bool operator == (const ICalculationUnit &other);

private:
	CalculationUnit(const CalculationUnit &other) {
		(void) other;
	}
	CalculationUnit(const std::string &name, const std::string & description, const std::string &unit);

	std::string name;
	std::string description;
	std::string unit;
};


#endif // CALCULATIONUNIT_H
