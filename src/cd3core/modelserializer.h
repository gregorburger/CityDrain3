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

#ifndef DESERIALIZIER_H
#define DESERIALIZIER_H


#include <QXmlDefaultHandler>
#include <map>
#include <cd3globals.h>
#include <boost/shared_ptr.hpp>
#include <boost/date_time.hpp>
using namespace boost::posix_time;

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

	void serialize(ptime time) const;
	void deserialize(ptime time);

private:
	void serialize(std::ostream &) const;
	void serializeNode(std::ostream &s, std::string &name, Node *n) const;

	std::string pathForTimeStep(ptime time) const;

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
