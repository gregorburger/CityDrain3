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

#ifndef SAXLOADER_H
#define SAXLOADER_H

#include <QXmlDefaultHandler>
#include <cd3globals.h>

struct SaxLoaderPriv;
class IModel;
class Node;
class ISimulation;
class NodeRegistry;
class SimulationRegistry;

class CD3_PUBLIC SaxLoader : public QXmlDefaultHandler
{
public:
	SaxLoader(IModel *m);
	SaxLoader(IModel *m, NodeRegistry *nr, SimulationRegistry *sr);
	~SaxLoader();
	bool startElement(const QString &ns,
					  const QString &lname,
					  const QString &qname,
					  const QXmlAttributes &atts);
	bool endElement(const QString &ns,
					const QString &lname,
					const QString &qname);
	bool error(const QXmlParseException &exception);
	bool fatalError(const QXmlParseException &exception);
	bool warning(const QXmlParseException &exception);
	bool load(QFile &f);
	ISimulation *getSimulation() const {
		return simulation;
	}
	IModel *getModel() const {
		return model;
	}
protected:
	IModel *model;
	ISimulation *simulation;
private:
	SaxLoaderPriv *pd;
	void loadParameter(const QXmlAttributes &atts);
	void breakCycle();
	Node *current;
	bool cycle_break;
	std::string source_id, source_port, sink_port, sink_id;
	bool delete_node_reg;
	bool delete_sim_reg;
};

#endif // SAXLOADER_H
