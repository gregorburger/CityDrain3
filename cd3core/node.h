#ifndef NODE_H
#define NODE_H
#include <string>
#include <map>
#include <cd3typeinfo.h>
#include <cassert>

#ifdef DEBUG
#include <iostream>
#endif

class Flow;

#define CD3_DECLARE_NODE(name)  \
class name : public Node { \
public: \
	const char *getNodeName() const {return #name;} \
private:

typedef std::map<std::string, Flow *>		ssf;
typedef std::pair<cd3::TypeInfo, void *>	ltvp;
typedef std::map<std::string, ltvp>			ssltvp;

#define ADD_PARAMETERS(var) #var, &var
#define ADD_PARAMETERS_P(var) #var, var

class Node
{
public:
	Node()
		: const_parameters(&parameters),
		const_states(&states),
		const_in_ports(&in_ports),
		const_out_ports(&out_ports)  {
	}

	virtual ~Node() {}

	virtual const char *getNodeName() const = 0;
	virtual void f(int time, int dt) = 0;

	//setup ports here called after all parameters are set
	//virtual void initPorts();
	virtual void init(int start, int end, int dt);
	virtual void deinit();

	virtual int getDT();

	void setInPort(const std::string &, const Flow *in);
	const Flow *getOutPort(const std::string &);

	template<class T> T *getState(const std::string &name) {
		assert(states.find(name) != states.end());
		ltvp p = states[name];
		assert(p.first == cd3::TypeInfo(typeid(T)));
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setState(const std::string &name, T &state) {
		assert(states.find(name) != states.end());
		ltvp p = states[name];
		assert(p.first == cd3::TypeInfo(typeid(T)));
		T *vp = static_cast<T*>(p.second);
		assert(vp);
		*vp = state;
	}

	template<class T>
	T *getParameter(const std::string &name) {
		assert(parameters.find(name) != parameters.end());
		ltvp p = parameters[name];
		assert(p.first == cd3::TypeInfo(typeid(T)));
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setParameter(const std::string &name,
					  T param) {
		assert(parameters.find(name) != parameters.end());
		ltvp p = parameters[name];

		assert(p.first == cd3::TypeInfo(typeid(T)));
		T *vp = static_cast<T*>(p.second);
		assert(vp);
		*vp = param;
	}

public:
	const ssltvp	* const const_parameters;
	const ssltvp	* const const_states;
	const ssf		* const const_in_ports;
	const ssf		* const const_out_ports;

protected:
	template<class T>
	void addState(const std::string &name, T *ptr) {
		assert(ptr);
		assert(states.find(name) == states.end());
		states[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

	template<class T>
	void addParameter(const std::string &name, T *ptr) {
		assert(ptr);
		assert(parameters.find(name) == parameters.end());
		parameters[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

	void addInPort	(const std::string &name, Flow *p);
	void addOutPort	(const std::string &name, Flow *p);

protected:
	ssltvp	states;
	ssltvp	parameters;
	ssf		in_ports;
	ssf		out_ports;
};

#endif // NODE_H
