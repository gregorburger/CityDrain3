#ifndef NODE_H
#define NODE_H
#include <string>
#include <map>
#include <cd3typeinfo.h>
#include <cd3assert.h>
#include <boost/format.hpp>

using namespace boost;

#ifdef DEBUG
#include <iostream>
#endif

class Flow;

#define CD3_DECLARE_NODE(node)  \
class node : public Node { \
public: \
	static const char *name; \
	const char *getClassName() const; \
private:

#define CD3_DECLARE_NODE_NAME(nodename) \
const char *nodename::name = #nodename; \
const char *nodename::getClassName() const { return nodename::name; }

typedef std::map<std::string, Flow *>		ssf;
typedef std::pair<cd3::TypeInfo, void *>	ltvp;
typedef std::map<std::string, ltvp>			ssltvp;

#define ADD_PARAMETERS(var) #var, &var
#define ADD_PARAMETERS_P(var) #var, var

class SimulationParameters;

class Node
{
	friend class ModelSerializer;
	friend class IStateMigrator;
public:
	Node()
		: const_parameters(&parameters),
		const_states(&states),
		const_in_ports(&in_ports),
		const_out_ports(&out_ports)  {
	}

	virtual ~Node() {}
	virtual int f(int time, int dt) = 0;

	virtual const char *getClassName() const = 0;
	//setup ports here called after all parameters are set
	//virtual void initPorts();
	virtual void init(int start, int end, int dt);
	virtual void deinit();

	/*virtual int getDT(const SimulationParameters &sp) const;
	virtual void setDT(int dt);*/

	//pull out states from a script
	//gets called before saving states
	virtual void pullOutStates(){}
	//push states into a script
	//gets called after states have been loaded
	virtual void pushInStates(){}

	void setInPort(const std::string &, const Flow *in);
	void setOutPort(const std::string &, const Flow *in);
	const Flow *getOutPort(const std::string &) const;

	template<class T> T *getState(const std::string &name) {
		cd3assert(states.find(name) != states.end(),
				  str(format("no state with name %1%") % name));
		ltvp p = states[name];
		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for state %1%") % name));
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setState(const std::string &name, T &state) {
		cd3assert(states.find(name) != states.end(),
				  str(format("no state with name %1%") % name));
		ltvp p = states[name];
		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for state %1%") % name));
		T *vp = static_cast<T*>(p.second);
		cd3assert(vp, str(format("state %1% null") % name));
		*vp = state;
	}

	template<class T>
	T *getParameter(const std::string &name) {
		cd3assert(parameters.find(name) != parameters.end(),
				  str(format("no parameter with name %1%") % name));
		ltvp p = parameters[name];
		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for parameter %1%") % name));
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setParameter(const std::string &name,
					  T param) {
		cd3assert(parameters.find(name) != parameters.end(),
				  str(format("no parameter with name %1%") % name));
		ltvp p = parameters[name];

		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for parameter %1%") % name));
		T *vp = static_cast<T*>(p.second);
		cd3assert(vp, str(format("parameter %1% null") % name));
		*vp = param;
	}

	const ssltvp	* const const_parameters;
	const ssltvp	* const const_states;
	const ssf		* const const_in_ports;
	const ssf		* const const_out_ports;

	template<class T>
	void addState(const std::string &name, T *ptr) {
		cd3assert(ptr, "adding null state");
		cd3assert(states.find(name) == states.end(),
				  str(format("state %1% already defined") % name));
		states[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

	template<class T>
	void addParameter(const std::string &name, T *ptr) {
		cd3assert(ptr, "adding nul parameter");
		cd3assert(parameters.find(name) == parameters.end(),
				  str(format("parameter %1% already defined") % name));
		parameters[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

protected:
	void addInPort	(const std::string &name, Flow *p);
	void addOutPort	(const std::string &name, Flow *p);

protected:
	ssltvp	states;
	ssltvp	parameters;
	ssf		in_ports;
	ssf		out_ports;
	int		dt;
};

#endif // NODE_H
