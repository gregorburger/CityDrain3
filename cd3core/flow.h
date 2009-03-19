#ifndef FLOW_H
#define FLOW_H

#include <boost/shared_ptr.hpp>
#include <vector>

class CalculationUnit;
class FlowDefinition;

typedef std::vector<double> FlowPriv;

class Flow {
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

	double getIth(const CalculationUnit *unit, size_t i) const;
	void setIth(const CalculationUnit *unit, size_t i, double value);

	const std::vector<std::string> &getNames() const;
	const std::vector<std::string> &getUnitNames(const CalculationUnit *unit) const;

	void copy();
	
	void dump() const;
private:
	void copyData();
	void copyDefinition();
	boost::shared_ptr<FlowPriv> f;
	boost::shared_ptr<FlowDefinition> fd;
};

void mix(Flow *out, const Flow *const *inputs, int num_inputs);
Flow *mix(const Flow * const *inputs, int num_inputs);
Flow mix(const std::vector<const Flow *> &inputs);
std::pair<Flow, Flow> split(const Flow f, float ratio);

#endif // FLOW_H
