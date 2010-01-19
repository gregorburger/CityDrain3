#include "simulationsaver.h"
#include <simulation.h>
#include <imodel.h>
#include <node.h>
#include <nodeconnection.h>
#include <flow.h>

#include <QXmlStreamWriter>
#include <QFile>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <string>
using namespace std;

SimulationSaver::SimulationSaver(ISimulation *simulation,
								 QString path,
								 QStringList plugins)
	: simulation(simulation), path(path), plugins(plugins) {
	out = new QFile(path);//leak
	out->open(QIODevice::WriteOnly);
	writer = new QXmlStreamWriter(out);
	writer->setAutoFormatting(true);
	//writer->setAutoFormattingIndent();
}

SimulationSaver::~SimulationSaver() {
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

	saveSimulation(simulation);
	saveModel(simulation->getModel());

	writer->writeEndElement();
	writer->writeEndDocument();

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
QString tos(string s) {
	return QString::fromStdString(s);
}
void SimulationSaver::saveSimulation(ISimulation *sim) {
	writer->writeStartElement("simulation");

	writer->writeAttribute("class", sim->getClassName());

	writer->writeStartElement("time");
	writer->writeAttribute("start", tos(sim->getSimulationParameters().start));
	writer->writeAttribute("start", tos(sim->getSimulationParameters().stop));
	writer->writeAttribute("dt", tos(sim->getSimulationParameters().dt));

	saveFlowDefinition();

	writer->writeEndElement(); //time
	writer->writeEndElement(); //simulation
}

void SimulationSaver::saveFlowDefinition() {
	writer->writeStartElement("flowdefinition");
	BOOST_FOREACH(string name, Flow::getNames()) {
		QString qname = QString::fromStdString(name);
		QString qunit = QString::fromStdString(cu2string(Flow::getUnit(name)));
		qunit = qunit.toLower();
		writer->writeEmptyElement(qunit);
		writer->writeAttribute("name", qname);
	}
	writer->writeEndElement();
}

void SimulationSaver::saveModel(IModel *model) {
	writer->writeStartElement("model");
	writer->writeStartElement("nodelist");

	const node_set_type *nset = model->getNodes();
	for (node_set_type::const_iterator it = nset->begin(); it != nset->end(); it++) {
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
	for (con_set_type::const_iterator it = cset->begin(); it != cset->end(); it++) {
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

void SimulationSaver::saveNodeParameters(const Node *n) {
	//TODO
}
