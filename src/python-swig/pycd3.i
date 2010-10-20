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
%}
%include std_vector.i
%include std_string.i
%include std_map.i

%feature("autodoc", "docstring");

%feature("director:except") {
	if ($error != NULL) {
		throw PythonException();
	}
}

namespace std {
	%template(vectors) vector<std::string>;
	%template(vectord) vector<double>;
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

class Flow {
public:
	enum CalculationUnit {
		flow,
		concentration,
		rain,
		volume,
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

	void __setitem__(int i, double d) {
		(*self)[i] = d;
	}
	int __len__() {
		return self->size();
	}
}
%nodefaultctor NodeParameter;
struct NodeParameter {
	NodeParameter &setDescription(std::string newdesc);
	NodeParameter &setUnit(std::string newunit);
};


%inline %{
class WrappedPrimitive {
public:
	WrappedPrimitive(){}
	~WrappedPrimitive(){}
};

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

%feature("pythonappend") Node::Node() %{
	self.prim_types = {float: WrappedDouble, int: WrappedInteger, str: WrappedString}
	self.wrapped = []
	self.added = []
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

	def addParameter(self, p):
		for k in self.__dict__:
			if p == self.__dict__[k]:
				name = k
				if issubclass(self.__dict__[k].__class__, WrappedPrimitive):
					name = name[2:]
				return self.intern_addParameter(name, self.__dict__[k])

	def addParameters(self):
		ignores = ["this"]
		for k in self.__dict__:
			if k in ignores:
				continue

			self.addParameter(k, self.__dict__[k])

	def __setattr__(self, name, value):
		if name in ["this", "prim_types", "wrapped", "added"]:
			self.__dict__[name] = value
			return

		if name not in self.wrapped and type(value) in self.prim_types.keys():
			self.__dict__["__"+name] = self.prim_types[type(value)](value)
			self.wrapped.append(name)
			return
		if name in self.wrapped:
			self.__dict__["__"+name].value = value
			return
		self.__dict__[name] = value

	def __getattr__(self, name):

		if name == "this":
			return self.__dict__[name]

		if "__"+name not in self.__dict__:
			raise AttributeError

		if name in self.wrapped and name not in self.added:
			self.added.append(name)
			return self.__dict__["__"+name]
		else:
			return self.__dict__["__"+name].value
		raise AttributeError
	%}
protected:
	void addInPort(std::string name, Flow *f);
	void addOutPort(std::string name, Flow *f);
};

%extend Node {
	NodeParameter &intern_addParameter(std::string name, std::vector<double> v) {
		return $self->addParameter(name, new std::vector<double>(v));
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
}

%inline %{
void log(std::string s) {
	Logger(Debug) << s;
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
		log("adding python node %s" % nf.getNodeName())
		nr.addNodeFactory(nf.__disown__())
%}
