#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
using namespace boost;
#include <flow.h>
#include <cd3globals.h>

namespace cd3 {
	class TypeInfo;
}

class Node;

struct IStateSerializer;

typedef std::map<cd3::TypeInfo, boost::shared_ptr<IStateSerializer> > type_ser_map;

struct CD3_PUBLIC IStateSerializer
{
	virtual ~IStateSerializer(){}
	virtual std::string serialize(const std::string &name, shared_ptr<Node> node) = 0;
	virtual void deserialize(const std::string &value, const std::string &name,  shared_ptr<Node> node) = 0;

	static type_ser_map standard;
};

struct CD3_PUBLIC FlowSerializer {
	static std::string toString(Flow f);
	static Flow fromString(const std::string &s);
};

#endif // SERIALIZER_H
