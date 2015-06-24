/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

#ifndef NODE_H
#define NODE_H

#include <string>
#include <map>
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
	static const char *class_name; \
	const char *getClassName() const; \
private:

#define CD3_DECLARE_NODE_NAME(nodename) \
const char *nodename::class_name = #nodename; \
const char *nodename::getClassName() const { return nodename::class_name; }

using namespace boost;

typedef std::map<std::string, Flow *>		ssf;
typedef std::pair<cd3::TypeInfo, void *>	ltvp;
typedef std::map<std::string, ltvp>			ssltvp;


#define ADD_PARAMETERS(var) #var, &var
#define ADD_PARAMETERS_P(var) #var, var

struct CD3_PUBLIC NodeParameter {
	NodeParameter(std::string name,
				  cd3::TypeInfo type,
				  void *value) : name(name), type(type), value(value) {
		unit = "-";
	}
	NodeParameter &setName(std::string newname) {
		name = newname;
		return *this;
	}

	NodeParameter &setDescription(std::string newdesc) {
		description = newdesc;
		return *this;
	}

	NodeParameter &setUnit(std::string newunit) {
		unit = newunit;
		return *this;
	}

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

	std::string getId() const;

	virtual const char *getClassName() const = 0;
	
	std::string getDescription() const {
		return description;
	}
	
	void setDescription(const std::string &d) {
		description = d;
	}

	/**
	  * init is called after parameters were set. Also
	  * if any kind of time parameter for the simulation
	  * has changed.
	  *
	  * init is used to acquire all ressources necessary for
	  * the given parameter set.
	  *
	  * allows to initialize dynamic parameters (e.g. given a mixer
	  * with dynamic input sizes to allow multiple in ports)
	  */
	virtual bool init(ptime start, ptime end, int dt);

	/**
	  * called before delete, ressources acquired in init
	  * should be deallocated in deinit.
	  *
	  * Also called when parameters changed so the node can restart into
	  * a clean state for the next init run.
	  */
	virtual void deinit();

	/**
	  * called at the beginning of each simulation run.
	  * When started from GUI start/stop may get called many
	  * times.
	  *
	  * used for opening files etc.
	  */
	virtual void start();

	/**
	  * called after each simulation run.
	  *
	  * used for closing files etc.
	  */
	virtual void stop();

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
		if (states.find(name) == states.end()) {
			Logger(Error) << str(format("no state with name %1%") % name);
			return 0;
		}
		ltvp p = states[name];
		if (p.first != cd3::TypeInfo(typeid(T))) {
			Logger(Error) << str(format("wrong type for state %1%") % name);
			return 0;
		}
		Logger(Debug) << this << "getState(" << name << ")";
		return static_cast<T*>(p.second);
	}

	template<class T>
	void setState(const std::string &name, T &state) {
		if (states.find(name) == states.end()) {
			Logger(Error) << str(format("no state with name %1%") % name);
			return;
		}
		ltvp p = states[name];
		if (p.first != cd3::TypeInfo(typeid(T))) {
			Logger(Error) << str(format("wrong type for state %1%") % name);
			return;
		}
		T *vp = static_cast<T*>(p.second);
		cd3assert(vp, str(format("state %1% null") % name));
		Logger(Debug) << this << "setState(" << name << ")";
		*vp = state;
	}

	template<class T>
	T *getParameter(const std::string &name) const {
		if (parameters.find(name) == parameters.end()) {
			Logger(Error) << str(format("no parameter with name %1%") % name);
			return 0;
		}
		NodeParameter *p = parameters.find(name)->second;
		if (p->type != cd3::TypeInfo(typeid(T))) {
			Logger(Error) << str(format("wrong type for parameter %1%") % name);
			return 0;
		}
		Logger(Debug) << this << "getParameter(" << name << ")";
		return static_cast<T*>(p->value);
	}

	template<class T>
	void setParameter(const std::string &name,
					  T param) {
		if (parameters.find(name) == parameters.end()) {
			Logger(Error) << str(format("no parameter with name %1%") % name);
			return;
		}

		NodeParameter *p = parameters[name];

		if (p->type != cd3::TypeInfo(typeid(T))) {
			Logger(Error) << str(format("wrong type for parameter %1%") % name);
			return;
		}

		T *vp = static_cast<T*>(p->value);
		cd3assert(vp, str(format("parameter %1% null") % name));
		Logger(Debug) << this << "setParameter(" << name << ")";
		*vp = param;
	}

	//const ssltvp	* const const_parameters;
	parameters_type getParameters() const {
		return parameters;
	}

	const ssltvp	* const const_states;
	const ssf		* const const_in_ports;
	const ssf		* const const_out_ports;

	template<class T>
	void addState(const std::string &name, T *ptr) {
		if (!ptr) {
			Logger(Error) << "adding null state";
			return;
		}
		if (states.find(name) != states.end()) {
			Logger(Error) << str(format("state %1% already defined") % name);
			return;
		}
		Logger(Debug) << this << "addState(" << name << ")";
		states[name] = ltvp(cd3::TypeInfo(typeid(T)), ptr);
	}

	void removeState(const std::string &name) {
		if (states.find(name) == states.end()) {
			Logger(Error) << str(format("state %1% not defined") % name);
			return;
		}
		Logger(Debug) << this << "removeState(" << name << ")";
		states.erase(name);
	}

	template<class T>
	NodeParameter &addParameter(const std::string &name, T *ptr) {
		if (!ptr) {
			Logger(Error) << "adding null parameter";
			return dummy;
		}

		if (parameters.count(name)) {
			Logger(Error) << str(format("parameter %1% already defined") % name);
			return dummy;
		}
		NodeParameter *p = new NodeParameter(name, cd3::TypeInfo(typeid(T)), ptr);
		Logger(Debug) << this << "addParameter(" << name << ")";
		parameters[name] = p;
		return *p;
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
	ssf		in_ports;
	ssf		out_ports;
	int		dt;
	std::string		id;
	std::string		description;
private:
	friend class MapBasedModel;
	void setId(const std::string &id);
	NodeParameter dummy;
};

#endif // NODE_H
