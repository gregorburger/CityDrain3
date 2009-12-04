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

struct FlowDefinition;

typedef std::vector<double> FlowPriv;


class CD3_PUBLIC Flow {
public:

	enum CalculationUnit {
		flow,
		concentration,
		rain,
		volume,
		null
	};

	Flow();
	Flow (const Flow &other);
	virtual ~Flow();

	Flow &operator =(const Flow &rhs);

	void addUnit(const std::string &name,
				 CalculationUnit unit,
				 double value);

	void setValue(const std::string &name,
				 double value);

	void clear();
	bool empty() const;

	double getValue(const std::string &name) const;
	CalculationUnit getUnit(const std::string &name) const;
	unsigned int countUnits(CalculationUnit unit) const;

	double getIth(CalculationUnit unit, size_t i) const;
	void setIth(CalculationUnit unit, size_t i, double value);

	const std::vector<std::string> &getNames() const;
	const std::vector<std::string> &getUnitNames(CalculationUnit unit) const;
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

std::string cu2string(Flow::CalculationUnit c);
Flow::CalculationUnit string2cu(std::string s);


#endif // FLOW_H
