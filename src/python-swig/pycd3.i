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
%}
%include std_vector.i
%include std_string.i
%include std_map.i

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

class Flow {
public:
	enum CalculationUnit {
		flow,
		concentration,
		rain,
		volume,
		null
	};

	Flow();
	static size_t size();
	static std::vector<std::string> getNames();
};

%extend Flow {
	double __getitem__(int i) const {
		return (*self)[i];
	}
	void __setitem__(int i, double d) {
		(*self)[i] = d;
	}
}

class Node {
public:
	Node();
	virtual ~Node();
	virtual int f(ptime current, int dt) = 0;
	virtual bool init(ptime start, ptime stop, int dt);
	virtual void deinit();
	virtual const char *getClassName() const = 0;
	virtual void updateParameters();

	%pythoncode %{
	def getClassName(self):
		return self.__class__.__name__

	def updateParameters(self):
		log("fuck")
		self.python_updateParameters(self)
	%}
protected:
	void addInPort(std::string name, Flow *f);
	void addOutPort(std::string name, Flow *f);
};

%extend Node {
	void addParameter(std::string name, std::string value) {
		$self->addParameter(name, new std::string(value));
	}

	void addParameter(std::string name, int value) {
		$self->addParameter(name, new int(value));
	}

	void addParameter(std::string name, std::vector<double> v) {
		$self->addParameter(name, new std::vector<double>(v));
	}

	void addParameter(std::string name, double value) {
		$self->addParameter(name, new double(value));
	}

	void python_updateParameters(PyObject *pself) {
		typedef std::pair<std::string, NodeParameter*> ppair;
		BOOST_FOREACH(ppair item, $self->getParameters()) {
			NodeParameter *p = item.second;
			TypeConverter *con = TypeConverter::get(p->type);
			if (!con) {
				Logger(Error) << "can not save value of parameter " << p->name;
				continue;
			}
			Logger(Debug) << "updating parameter" << p->name;
			con->updatePythonParameter(pself, p);
		}
	}
}

%inline %{
void log(std::string s) {
	Logger(Debug) << s;
}
%}

class INodeFactory {
public:
	virtual ~INodeFactory();
	virtual Node *createNode() const = 0;
	virtual std::string getNodeName() const = 0;
	virtual std::string getSource() const = 0;
};

class NodeRegistry
{
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
%}
