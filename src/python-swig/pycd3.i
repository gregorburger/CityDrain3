%module(directors="1", allprotected="1") pycd3
%feature("director");
%{
#include <nodefactory.h>
#include <noderegistry.h>
#include <node.h>
#include <flow.h>
#include <iostream>
#include <pythonexception.h>
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
protected:
	void addInPort(std::string name, Flow *f);
	void addOutPort(std::string name, Flow *f);
};

%extend Node {
	const char *getClassName() const {
		return "";
	}
}

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
