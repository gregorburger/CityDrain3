%module(directors="1", allprotected="1") pycd3
%feature("director");
%{
#include <nodefactory.h>
#include <noderegistry.h>
#include <node.h>
#include <flow.h>
#include <iostream>
#include <string>
#include <pythonexception.h>
#include <boost/foreach.hpp>
#include <typeconverter.h>
#include <boost/date_time.hpp>

class WrappedPrimitive {
protected:
	WrappedPrimitive(){}
public:
	~WrappedPrimitive(){}
};

%}
%include std_vector.i
%include std_string.i
%include std_map.i

%feature("autodoc", "docstring");

%feature("director:except") {
	if ($error != NULL) {
		//PyErr_Print();
		throw PythonException();
	}
}

namespace std {
	%template(DoubleVector) vector<double>;
	%template(IntegerVector) vector<int>;
	%template(StringVector) vector<std::string>;
	%template(FlowVector) vector<Flow *>;
};

%exception {
	try {
		$action
	} catch (...) {
		Logger(Error) << "FIXME: handle correctly::Exception happened";
		abort();
	}
}

%typemap(out) ptime {
	$result = to_simple_string($1);
}

%nodefaultctor;

namespace boost {
	namespace posix_time {
		class ptime {
		public:
			boost::gregorian::date date() const;
			boost::posix_time::time_duration time_of_day() const;
			%pythoncode %{
				def to_datetime(self):
					from datetime import datetime
					d = self.date()
					t = self.time_of_day()
					return datetime(d.year(), d.month(), d.day(), t.hours(), t.minutes(), t.seconds())
			%}
		};

		class time_duration {
		public:
			int hours() const;
			int minutes() const;
			int seconds() const;
		};
	}
	namespace gregorian {
		class date {
		public:
			int year() const;
			int month() const;
			int day() const;
		};
	}
}

%clearnodefaultctor;

%nodefaultctor NodeParameter;
struct NodeParameter {
	NodeParameter &setDescription(std::string newdesc);
	NodeParameter &setUnit(std::string newunit);
};

class WrappedPrimitive {
private:
	WrappedPrimitive();
public:
	~WrappedPrimitive();
	%pythoncode %{

	def __lt__(self, other):
		return self.value < other
	def __le__(self, other):
		return self.value <= other
	def __eq__(self, other):
		return self.value == other
	def __ne__(self, other):
		return self.value != other
	def __gt__(self, other):
		return self.value > other
	def __ge__(self, other):
		return self.value >= other

	def __div__(self, other):
		return self.value / other
	def __add__(self, other):
		return self.value + other
	def __sub__(self, other):
		return self.value - other
	def __mul__(self, other):
		return self.value * other
	def __floordiv__(self, other):
		return self.value // other
	def __mod__(self, other):
			return self.value % other
	def __divmod__(self, other):
		return divmod(self.value, other)
	def __pow__(self, other, modulo = None):
		if (modulo == None):
			return self.value ** other
		else:
			return pow(self.value, other, modulo)
	def __lshift__(self, other):
		return self.value << other
	def __rshift__(self, other):
		return self.value >> other
	def __and__(self, other):
		return self.value & other
	def __xor__(self, other):
		return self.value ^ other
	def __or__(self, other):
		return self.value | other

	def __radd__(self, other):
		return other + self.value
	def __rsub__(self, other):
		return other - self.value
	def __rmul__(self, other):
		return other * self.value
	def __rdiv__(self, other):
		return other / self.value

	def __rfloordiv__(self, other):
		return other // self.value
	def __rmod__(self, other):
		return other % self.value
	def __rdivmod__(self, other):
		return divmod(other, self.value)
	def __rpow__(self, other):
		return other ** self.value
	def __rlshift__(self, other):
		return other << self.value
	def __rrshift__(self, other):
		return other >> self.value
	def __rand__(self, other):
		return other & self.value
	def __rxor__(self, other):
		return other ^ self.value
	def __ror__(self, other):
		return other | self.value

	def __rdiv__(self, left):
		return left / self.value

	def __req__(self, left):
		return left == self.value

	def __str__(self):
		return str(self.value)

	def __float(self):
		return float(self.value)

	def __int__(self):
		return int(self.value)

	def __long__(self):
		return long(self.value)
	%}
};

%rename (Double) WrappedDouble;
%rename (Integer) WrappedInteger;
%rename (String) WrappedString;

%inline %{
class WrappedDouble : public WrappedPrimitive {
public:
	explicit WrappedDouble(double value) : value(value) {
	}
	double value;
};

class WrappedInteger : public WrappedPrimitive {
public:
	explicit WrappedInteger(int value) : value(value) {
	}
	int value;
};

class WrappedString : public WrappedPrimitive {
public:
	explicit WrappedString(std::string value) : value(value) {
	}
	std::string value;
};
%}

class Flow {
public:
	enum CalculationUnit {
		flow,
		concentration,
		rain,
		volume,
		climatic,
		temperature,
		null
	};

	Flow(const Flow&);
	Flow();
	static size_t size();
	static std::vector<std::string> getNames();
	static size_t countUnits(CalculationUnit unit);
	%pythoncode %{
	def __iter__(self):
		for i in xrange(self.size()):
			yield self[i]
	%}
};

%extend Flow {
	double __getitem__(int i) const {
		return (*self)[i];
	}

	PyObject* __getitem__(PyObject *slice) const {
		Py_ssize_t length, start, stop, step;
		if (!PySlice_Check(slice)) {
			return Py_None;
		}
		length = Flow::size();
		PySlice_GetIndices((PySliceObject*)slice, length, &start, &stop, &step);
		PyObject *list = PyList_New(0);
		for (int i = start; i < stop; i+= step) {
			PyObject *number = PyFloat_FromDouble((*self)[i]);
			PyList_Append(list, number);
		}
		return list;
	}

	void __setitem__(int i, WrappedDouble d) {
		(*self)[i] = d.value;
	}


	void __setitem__(int i, double d) {
		(*self)[i] = d;
	}

	int __len__() {
		return self->size();
	}
}

%pythonprepend Node::Node %{
	self.wrapped_values = {}
%}

class Node {
public:
	Node();
	virtual ~Node();
	virtual int f(boost::posix_time::ptime current, int dt) = 0;
	virtual bool init(boost::posix_time::ptime start, boost::posix_time::ptime stop, int dt);
	virtual void deinit();
	virtual const char *getClassName() const = 0;

	%pythoncode %{
	def getClassName(self):
		return self.__class__.__name__

	def addParameter(self, *args):
		if len(args) > 2:
			raise TypeError("either addParameter(name, parameter) or addParameter(parameter)")
		if len(args) == 2:
			p = args[1]
			name = args[0]
		else:
			p = args[0]
			for k in self.__dict__:
				if self.__dict__[k] == p:
					name = k
					break

		if p.__class__ not in [Integer, Double, String, Flow, DoubleVector, IntegerVector, StringVector]:
			msg = "parameter %s has wrong type %s\n" % (name, p.__class__)
			msg += " parameters can only have type Integer(), Double(), String(), Flow, DoubleVector(), IntegerVector() or StringVector()"
			raise TypeError(msg)

		log("adding parameter %s with type %s" % (name, p.__class__))
		self.wrapped_values[name] = p
		return self.intern_addParameter(name, p)

	def addState(self, *args):
		if len(args) > 2:
			raise TypeError("either addState(name, parameter) or addState(parameter)")
		if len(args) == 2:
			state = args[1]
			name = args[0]
		else:
			state = args[0]
			for k in self.__dict__:
				if self.__dict__[k] == state:
					name = k
					break
		nt = [Integer, Double, String, Flow, DoubleVector]
		if state.__class__ not in nt:
			msg = "state %s has wrong type" % name
			msg += "states can only have type Integer(), Double(), String(), Flow, DoubleVector(), IntegerVector(), StringVector(), FlowVector()"
			raise TypeError(msg)

		log("adding parameter %s with type %s" % (name, state.__class__))
		self.wrapped_values[name] = state
		self.intern_addState(name, state)

	def addParameters(self):
		ignores = ["this", "wrapped_values"]
		for k in self.__dict__:
			if k in ignores:
				continue

			self.addParameter(k, self.__dict__[k])

	def __setattr__(self, name, value):
		if "wrapped_values" in self.__dict__ and name in self.wrapped_values:
			self.__dict__[name].value = value

		#call copy constructor of flow class
		if name in self.__dict__ and value.__class__ == Flow and self.__dict__[name].__class__ == Flow:
			assign(self.__dict__[name], value)

		self.__dict__[name] = value

	%}
protected:
	void addInPort(std::string name, Flow *f);
	void removeInPort(std::string name);
	void addOutPort(std::string name, Flow *f);
	void removeOutPort(std::string name);
};

%extend Node {
	NodeParameter &intern_addParameter(std::string name, std::vector<double> *v) {
		return $self->addParameter(name, v);
	}

	NodeParameter &intern_addParameter(std::string name, Flow *v) {
		return $self->addParameter(name, v);
	}

	NodeParameter &intern_addParameter(std::string name, WrappedDouble *value) {
		return $self->addParameter(name, &value->value);
	}

	NodeParameter &intern_addParameter(std::string name, WrappedInteger *value) {
		return $self->addParameter(name, &value->value);
	}

	NodeParameter &intern_addParameter(std::string name, WrappedString *value) {
		return $self->addParameter(name, &value->value);
	}

	void intern_addState(std::string name, std::vector<double> *v) {
		$self->addState(name, v);
	}
/*
// no serializers for vectors for now
	void intern_addState(std::string name, std::vector<Flow *> *v) {
		$self->addState(name, v);
	}


	void intern_addState(std::string name, std::vector<int> *v) {
		$self->addState(name, v);
	}

	void intern_addState(std::string name, std::vector<std::string> *v) {
		$self->addState(name, v);
	}
*/
	void intern_addState(std::string name, Flow *v) {
		$self->addState(name, v);
	}

	void intern_addState(std::string name, WrappedDouble *value) {
		$self->addState(name, &value->value);
	}

	void intern_addState(std::string name, WrappedInteger *value) {
		$self->addState(name, &value->value);
	}

	void intern_addState(std::string name, WrappedString *value) {
		$self->addState(name, &value->value);
	}
}

enum LogLevel {
	Debug = 0,
	Standard = 1,
	Warning = 2,
	Error = 3
};

%inline %{
void log(std::string s, LogLevel l = Debug) {
	Logger(l) << s;
}
void assign(Flow *target, Flow *source) {
	for (size_t i = 0; i < Flow::size(); i++) {
		*target = *source;
	}
}
%}

class INodeFactory {
public:
	virtual ~INodeFactory();
	virtual Node *createNode() const = 0;
	virtual std::string getNodeName() const = 0;
	virtual std::string getSource() const = 0;
};

class NodeRegistry {
public:
	NodeRegistry();
	~NodeRegistry();

	bool addNodeFactory(INodeFactory *factory);
	std::vector<std::string> getRegisteredNames() const;

	std::map<std::string, std::vector<std::string> >
	getRegisteredNamesAndSources() const;

	Node *createNode(const std::string &name) const;
	bool contains(const std::string &name) const;
};

%pythoncode %{
class Redirector:
	def __init__(self, orig_out, log_level):
		self.orig_out = orig_out
		self.log_level = log_level
		self.currentstring = ""

	def write(self, text):
		self.orig_out.write(text + "\n")
		self.currentstring = self.currentstring + " " + text

		if text.rfind("\n") == -1:
			return

		self.currentstring=self.currentstring.replace("\n","")
		self.currentstring=self.currentstring.replace("\r","")

		log("PYTHON:" + self.currentstring, self.log_level)
		
		self.currentstring=""

	def close(self):
		self.orig_out.close()

def install_redirector():
	import sys
	if not isinstance(sys.stdout, Redirector):
		sys.stdout=Redirector(sys.stdout, Debug)
		sys.stderr=Redirector(sys.stderr, Error)

class NodeFactory(INodeFactory):
	def __init__(self, node):
		INodeFactory.__init__(self)
		self.node = node

	def getNodeName(self):
		return self.node.__name__

	def createNode(self):
		n = self.node()
		n.__disown__()
		return n

	def getSource(self):
		return "testnodes.py"

def registerAllNodes(nr):
	for c in Node.__subclasses__():
		nf = NodeFactory(c)
		if nr.contains(nf.getNodeName()):
			continue
		log("adding python node %s" % nf.getNodeName())
		nr.addNodeFactory(nf.__disown__())
%}
