#ifndef FLOW_H
#define FLOW_H

#ifdef USE_NON_SHARED_FLOW
#else
#define SHARED_FLOW
#endif

#ifdef SHARED_FLOW
#include <boost/shared_ptr.hpp>
#endif
#include <vector>
#include <cd3globals.h>
#include <string>

class CalculationUnit;
struct FlowDefinition;

typedef std::vector<double> FlowPriv;

class CD3_PUBLIC Flow {
public:
	Flow();
	Flow (const Flow &other);
	virtual ~Flow();

	Flow &operator =(const Flow &rhs);

	void addUnit(const std::string &name,
				 const CalculationUnit *unit,
				 double value);

	void setValue(const std::string &name,
				 double value);

	void clear();
	bool empty() const;

	double getValue(const std::string &name) const;
	const CalculationUnit *getUnit(const std::string &name) const;
	unsigned int countUnits(const CalculationUnit *unit) const;

	double getIth(const CalculationUnit *unit, size_t i) const;
	void setIth(const CalculationUnit *unit, size_t i, double value);

	const std::vector<std::string> &getNames() const;
	const std::vector<std::string> &getUnitNames(const CalculationUnit *unit) const;
	bool hasName(const std::string &name) const;

//	void copy();
	void dump() const;

	static Flow nullFlow();
private:
	void copyData();
	void copyDefinition();
#ifdef SHARED_FLOW
	boost::shared_ptr<FlowPriv> f;
	boost::shared_ptr<FlowDefinition> fd;
#else
	FlowPriv *f;
	FlowDefinition *fd;
#endif
};

#endif // FLOW_H