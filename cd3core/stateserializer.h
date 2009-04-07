#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include <flow.h>

namespace cd3 {
	struct TypeInfo;
}

class Node;

struct IStateSerializer;

typedef std::map<cd3::TypeInfo, boost::shared_ptr<IStateSerializer> > type_ser_map;

struct IStateSerializer
{
	virtual ~IStateSerializer(){}
	virtual std::string serialize(const std::string &name, Node *node) = 0;
	virtual void deserialize(const std::string &value, const std::string &name,  Node *node) = 0;

	static type_ser_map standard;
};

struct FlowSerializer {
	static std::string toString(Flow f);
	static Flow fromString(const std::string &s);
};

#endif // SERIALIZER_H
