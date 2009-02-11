#ifndef NODE_H
#define NODE_H

#include <string>
#include <map>
#include <list>
#include <utility>
#include <loki/LokiTypeInfo.h>
#include <boost/shared_ptr.hpp>

class Flow;

#define CD3_DECLARE_NODE(name)  \
class name : public Node { \
public: \
	const char *getNodeName() const {return #name;} \
private:

typedef std::map<std::string, Flow *>		ssf;
typedef std::pair<Loki::TypeInfo, void *>	ltvp;
typedef std::map<std::string, ltvp>			ssltvp;

#define ADD_PARAMETERS(var) #var, &var
#define ADD_PARAMETERS_P(var) #var, var

class Node
{
public:
	Node() :
	const_parameters(&parameters),
	const_states(&states),
	const_in_ports(&in_ports),
	const_out_ports(&out_ports)  {
	}
	~Node() {}

	virtual const char *getNodeName() const = 0;
	virtual void f(int time, int dt) = 0;

	virtual void init(int start, int end, int dt) {
		(void) start;
		(void) end;
		(void) dt;
	}
	virtual void deinit() {
	}

	virtual std::pair<int,int> getMinMaxDT() { return std::make_pair<int, int>(-1, -1); }
	static std::pair<int, int>  MIN_MAX_DT_DONT_CARE;

	void setInPort(const std::string &, const Flow *in);

	template<class T> bool setState(const std::string &name, boost::shared_ptr<T> state) {
		ltvp p = states[name];
		if (p.first != typeid(T))
			return false;
		T *vp = static_cast<T*>(p.second);
		assert(vp);
		*vp = *(state.get());
		return true;
	}

	template<class T> bool setParameter(const std::string &name, boost::shared_ptr<T> param) {
		ltvp p = parameters[name];
		if (p.first != typeid(T))
			return false;
		T *vp = static_cast<T*>(p.second);
		assert(vp);
		*vp = *(param.get());
		return true;
	}

public:
	const ssltvp	* const const_parameters;
	const ssltvp	* const const_states;
	const ssf		* const const_in_ports;
	const ssf		* const const_out_ports;

protected:
	template<class T>
	void addState(const std::string &name, T *ptr) {
		states[name] = ltvp(Loki::TypeInfo(typeid(T)), ptr);
	}

	template<class T>
	void addParameter(const std::string &name, T *ptr) {
		parameters[name] = ltvp(Loki::TypeInfo(typeid(T)), ptr);
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
