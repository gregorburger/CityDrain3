%module(directors="1", allprotected="1") pycd3
%feature("director");
%{
#include <nodefactory.h>
#include <noderegistry.h>
#include <node.h>
#include <flow.h>
%}
%include std_vector.i
%include std_string.i
%include std_map.i

%typemap(out) ptime {
	$result = to_simple_string($1);
}

class Flow {
public:
	Flow();
};

class Node {
public:
	Node();
	virtual ~Node();
	virtual int f(ptime current, int dt) = 0;
	virtual bool init(ptime start, ptime stop, int dt);
	virtual const char *getClassName() const = 0;
protected:
	void addInPort(std::string name, Flow *f);
	void addOutPort(std::string name, Flow *f);
};

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
