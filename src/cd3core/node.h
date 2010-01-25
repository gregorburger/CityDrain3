#ifndef NODE_H
#define NODE_H

#include <string>
#include <boost/unordered/unordered_map.hpp>
#include <cd3typeinfo.h>
#include <cd3assert.h>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
using namespace boost::posix_time;

#include <cd3globals.h>

#ifdef DEBUG
#include <iostream>
#endif

class Flow;

#define CD3_DECLARE_NODE(node)  \
class CD3_PUBLIC node : public Node { \
public: \
	static const char *name; \
	const char *getClassName() const; \
private:

#define CD3_DECLARE_NODE_NAME(nodename) \
const char *nodename::name = #nodename; \
const char *nodename::getClassName() const { return nodename::name; }

using namespace boost;

typedef unordered_map<std::string, Flow *>		ssf;
typedef std::pair<cd3::TypeInfo, void *>	ltvp;
typedef unordered_map<std::string, ltvp>			ssltvp;


#define ADD_PARAMETERS(var) #var, &var
#define ADD_PARAMETERS_P(var) #var, var

struct CD3_PUBLIC NodeParameter {
	NodeParameter(std::string name,
				  cd3::TypeInfo type,
				  void *value) : name(name), type(type), value(value) {}
	std::string name, description, unit;
	cd3::TypeInfo type;
	void *value;
};

typedef std::map<std::string, NodeParameter*> parameters_type;

class CD3_PUBLIC Node
{
	friend class ModelSerializer;
	friend class IStateMigrator;
public:
	Node();

	virtual ~Node();
	virtual int f(ptime time, int dt) = 0;

	void setId(const std::string &id);
	std::string getId() const;

	virtual const char *getClassName() const = 0;
	virtual bool init(ptime start, ptime end, int dt);
	/*
	  called before delete, ressources acquired in init
	  should be deallocated in deinit.
	  */
	virtual void deinit();

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
		Logger(Debug) << this << "getState(" << name << ")";
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
		Logger(Debug) << this << "setState(" << name << ")";
		*vp = state;
	}

	template<class T>
	T *getParameter(const std::string &name) {
		cd3assert(parameters.find(name) != parameters.end(),
				  str(format("no parameter with name %1%") % name));
		ltvp p = parameters[name];
		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for parameter %1%") % name));
		Logger(Debug) << this << "getParameter(" << name << ")";
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setParameter(const std::string &name,
					  T param) {
		cd3assert(parameters.find(name) != parameters.end(),
				  str(format("no parameter with name %1%") % name));
		NodeParameter *p = parameters[name];

		cd3assert(p->type == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for parameter %1%") % name));
		T *vp = static_cast<T*>(p->value);
		cd3assert(vp, str(format("parameter %1% null") % name));
		Logger(Debug) << this << "setParameter(" << name << ")";
		*vp = param;
	}

	template<class T>
	void appendArrayParameter(const std::string &name,
							  T param) {
		cd3assert(array_parameters.find(name) != array_parameters.end(),
				  str(format("no parameter with name %1%") % name));
		ltvp p = array_parameters[name];

		cd3assert(p.first == cd3::TypeInfo(typeid(T)),
				  str(format("wrong type for parameter %1%") % name));

		vector<T> *vp = static_cast<vector<T> *>(p.second);
		cd3assert(vp, str(format("parameter %1% null") % name));
		Logger(Debug) << this << "appendArrayParameter(" << name << ")";
		vp->push_back(param);
	}


	//const ssltvp	* const const_parameters;
	parameters_type getParameters() {
		return parameters;
	}

	const ssltvp	* const const_array_parameters;
	const ssltvp	* const const_states;
	const ssf		* const const_in_ports;
	const ssf		* const const_out_ports;

	template<class T>
	void addState(const std::string &name, T *ptr) {
		cd3assert(ptr, "adding null state");
		cd3assert(states.find(name) == states.end(),
				  str(format("state %1% already defined") % name));
		Logger(Debug) << this << "addState(" << name << ")";
		states[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

	template<class T>
	void addParameter(const std::string &name, T *ptr) {
		cd3assert(ptr, "adding null parameter");
		cd3assert(!parameters.count(name), str(format("parameter %1% already defined") % name));
		NodeParameter *p = new NodeParameter(name, cd3::TypeInfo(typeid(T)), ptr);
		Logger(Debug) << this << "addParameter(" << name << ")";
		parameters[name] = p;
	}

	template<class T>
	void addArrayParameter(const std::string &name,
						   std::vector<T> *v) {
		cd3assert(v, "adding null array parameter");
		cd3assert(array_parameters.find(name) == array_parameters.end(),
				  str(format("parameter %1% already defined") % name));
		Logger(Debug) << this << "addArrayParameter(" << name << ")";
		array_parameters[name] = ltvp(cd3::TypeInfo(typeid(T)), v);
	}

	int num_inputed;

protected:
	void addInPort	(const std::string &name, Flow *p);
	void removeInPort	(const std::string &name);
	void addOutPort	(const std::string &name, Flow *p);
	void removeOutPort	(const std::string &name);

protected:
	ssltvp	states;
	std::map<std::string, NodeParameter*> parameters;
	ssltvp	array_parameters;
	ssf		in_ports;
	ssf		out_ports;
	int		dt;
	std::string		id;
};

#endif // NODE_H
