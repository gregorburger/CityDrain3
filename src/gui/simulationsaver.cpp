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

#include <typeconverter.h>
#include "simulationsaver.h"
#include <simulation.h>
#include <imodel.h>
#include <node.h>
#include <nodeconnection.h>
#include <flow.h>

#include <simulationscene.h>

#include <QXmlStreamWriter>
#include <QFile>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <string>

SimulationSaver::SimulationSaver(SimulationScene *scene,
								 QString path,
								 QStringList plugins,
								 QStringList python_modules,
								 QStringList python_controller)
	: scene(scene), path(path),
	  plugins(plugins),
	  python_modules(python_modules),
	  python_controller(python_controller) {
	out = new QFile(path);
	out->open(QIODevice::WriteOnly);
	writer = new QXmlStreamWriter(out);
	writer->setAutoFormatting(true);
}

SimulationSaver::~SimulationSaver() {
	delete writer;
	delete out;
}

void SimulationSaver::save() {
	writer->writeStartDocument("1.0");
	writer->writeStartElement("citydrain");
	writer->writeAttribute("version", "1.0");

	Q_FOREACH(QString plugin, plugins) {
		writer->writeEmptyElement("pluginpath");
		writer->writeAttribute("path", plugin);
	}

	Q_FOREACH(QString module, python_modules) {
		writer->writeEmptyElement("pythonmodule");
		writer->writeAttribute("module", module);
	}

	saveSimulation(scene->getSimulation());
	saveModel(scene->getSimulation()->getModel());
	saveNodePositions(scene->getNodeItems().values());

	writer->writeEndElement();
	writer->writeEndDocument();
	out->close();
}

inline
QString tos(int dt) {
	return QString("%1").arg(dt);
}

inline
QString tos(ptime t) {
	return QString::fromStdString(to_simple_string(t));
}

inline
QString tos(std::string s) {
	return QString::fromStdString(s);
}

void SimulationSaver::saveSimulation(ISimulation *sim) {
	writer->writeStartElement("simulation");

	writer->writeAttribute("class", sim->getClassName());

	writer->writeStartElement("time");
	writer->writeAttribute("start", tos(sim->getSimulationParameters().start));
	writer->writeAttribute("stop", tos(sim->getSimulationParameters().stop));
	writer->writeAttribute("dt", tos(sim->getSimulationParameters().dt));
	writer->writeEndElement(); //time

	saveFlowDefinition();

	BOOST_FOREACH(QString controller, python_controller) {
		writer->writeStartElement("controller");
		writer->writeAttribute("path", controller);
		writer->writeEndElement();
	}

	writer->writeEndElement(); //simulation
}

void SimulationSaver::saveFlowDefinition() {
	writer->writeStartElement("flowdefinition");
	BOOST_FOREACH(std::string name, Flow::getNames()) {
		QString qname = QString::fromStdString(name);
		QString qunit = QString::fromStdString(cu2string(Flow::getUnit(name)));
		qunit = qunit.toLower();
		writer->writeEmptyElement(qunit);
		writer->writeAttribute("name", qname);
	}
	writer->writeEndElement();
}

void SimulationSaver::saveModel(IModel *model) {
	Q_ASSERT(model);
	writer->writeStartElement("model");
	writer->writeStartElement("nodelist");

	const node_set_type *nset = model->getNodes();
	for (node_set_type::const_iterator it = nset->begin(); it != nset->end(); ++it) {
		const Node *n = *it;
		writer->writeStartElement("node");
		writer->writeAttribute("id", tos(n->getId()));
		writer->writeAttribute("class", tos(n->getClassName()));
		saveNodeParameters(n);
		writer->writeEndElement();
	}
	writer->writeEndElement(); //nodelist

	writer->writeStartElement("connectionlist");
	const con_set_type *cset = model->getConnections();
	int con_count = 0;
	for (con_set_type::const_iterator it = cset->begin(); it != cset->end(); ++it) {
		const NodeConnection *con = *it;
		writer->writeStartElement("connection");
		writer->writeAttribute("id", tos(con_count++));

		writer->writeEmptyElement("source");
		writer->writeAttribute("node", tos(con->source->getId()));
		writer->writeAttribute("port", tos(con->source_port));

		writer->writeEmptyElement("sink");
		writer->writeAttribute("node", tos(con->sink->getId()));
		writer->writeAttribute("port", tos(con->sink_port));

		writer->writeEndElement();//connection
	}

	writer->writeEndElement();//connectionlist
	writer->writeEndElement();//model
}

typedef std::pair<std::string, NodeParameter*> ppair;
void SimulationSaver::saveNodeParameters(const Node *n) {
	BOOST_FOREACH(ppair item, n->getParameters()) {
		NodeParameter *p = item.second;
		TypeConverter *con = TypeConverter::get(p->type);
		if (!con) {
			Logger(Error) << "can not save value of parameter " << p->name;
			continue;
		}
		writer->writeEmptyElement("parameter");
		writer->writeAttribute("name", tos(p->name));
		writer->writeAttribute("type", tos(con->getTypeName()));
		writer->writeAttribute("value", tos(con->toString(p->value)));
	}
}

void SimulationSaver::saveFlowParameter(NodeParameter *p) {
	Flow *f = (Flow*) p->value;
	writer->writeStartElement("flow");


	BOOST_FOREACH(std::string name, Flow::getNames()) {
		writer->writeEmptyElement("unit");
		writer->writeAttribute("name", QString::fromStdString(name));
		writer->writeAttribute("value", QString("%1").arg(f->getValue(name)));
	}

	writer->writeEndElement();
}

void SimulationSaver::saveNodePositions(QList<NodeItem *>items) {
	writer->writeStartElement("gui");

	Q_FOREACH(NodeItem *item, items) {
		writer->writeEmptyElement("nodeposition");
		writer->writeAttribute("id", item->getId());
		QPointF scenePos = item->scenePos();
		writer->writeAttribute("x", QString("%1").arg(scenePos.x()));
		writer->writeAttribute("y", QString("%1").arg(scenePos.y()));
	}

	writer->writeEndElement();
}
