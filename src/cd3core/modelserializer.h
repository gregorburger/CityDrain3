#ifndef DESERIALIZIER_H
#define DESERIALIZIER_H

#include <QXmlDefaultHandler>
#include <boost/shared_ptr.hpp>
using namespace boost;
#include <map>
#include <cd3globals.h>

class IModel;

namespace cd3 {
	class TypeInfo;
}

struct IStateSerializer;
class Node;

class CD3_PUBLIC ModelSerializer {
public:
	ModelSerializer(IModel *model, const std::string &dir);
	virtual ~ModelSerializer();

	void serialize(int time) const;
	void deserialize(int time);

private:
	void serialize(std::ostream &) const;
        void serializeNode(std::ostream &s, std::string &name, Node *n) const;

	std::string pathForTimeStep(int time) const;

	IModel *model;
	std::string dir;
};

class Deserializer : public QXmlDefaultHandler {
public:
	Deserializer(IModel *m);
	~Deserializer();

	bool startElement(const QString & namespaceURI,
					  const QString & localName,
					  const QString & qName,
					  const QXmlAttributes & atts);

	bool endElement(const QString &namespaceURI,
					const QString &localName,
					const QString &qName);

	bool characters(const QString & ch);

	bool error(const QXmlParseException &e);
	bool fatalError(const QXmlParseException &e);

private:
	IModel *model;
        Node *current;
	std::string stateName;
	std::string stateType;
	std::string portName;
	bool in_state_node;
	bool in_outport;
	bool in_inport;
	std::map<cd3::TypeInfo, boost::shared_ptr<IStateSerializer> > mapper;
};

#endif // DESERIALIZIER_H
