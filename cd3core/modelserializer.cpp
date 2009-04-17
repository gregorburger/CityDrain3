#include "modelserializer.h"
#include "stateserializer.h"
#include "cd3typeinfo.h"
#include "node.h"
#include <model.h>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QFile>
#include <QDir>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <cd3assert.h>

typedef std::pair<std::string, Node*> snp;
typedef std::pair<std::string, ltvp> sltvpp;
typedef std::pair<std::string, Flow *> ssfp;

ModelSerializer::ModelSerializer(IModel *m, const std::string &dir)
: model(m), dir(dir) {

}

ModelSerializer::~ModelSerializer() {
}

std::string ModelSerializer::pathForTimeStep(int time) const {
	return boost::str(boost::format("%2%/%1%-cd3-state.xml") % time % dir);
}

void ModelSerializer::serialize(int time) const {
	QString qs_dir = QString::fromStdString(dir);
	QDir qdir = QDir::current();
	if (!qdir.exists(qs_dir)) {
		qdir.mkdir(qs_dir);
	}

	std::string path = pathForTimeStep(time);
	std::ofstream out(path.c_str());
	serialize(out);
	out.close();
}

void ModelSerializer::serialize(std::ostream &os) const {
	os << "<model>" << std::endl;
        name_node_map names_nodes = model->getNamesAndNodes();
	BOOST_FOREACH(snp node, names_nodes) {
		serializeNode(os, node.first, node.second);
	}
	os << "</model>" << std::endl;
}

void ModelSerializer::serializeNode(std::ostream &os, std::string &node_name, Node *n) const {
	os << boost::format("\t<node name=\"%1%\" class=\"%2%\">\n") % node_name % n->getClassName();
	type_ser_map &serializers = IStateSerializer::standard;

	n->pullOutStates();

	BOOST_FOREACH(sltvpp state, n->states) {
		std::string name = state.first;
		cd3::TypeInfo type(state.second.first);
		cd3assert(serializers.find(type) != serializers.end(), "no serializer for the type");
		os << boost::format("\t\t<state name=\"%1%\" type=\"%2%\">\n") % name % type.type_name;
		os << "\t\t\t" << serializers[type]->serialize(name, n) << "\n";
		os << "\t\t</state>\n";
	}

	boost::shared_ptr<IStateSerializer> flow_ser = serializers[cd3::TypeInfo(typeid(Flow))];

	BOOST_FOREACH(ssfp in_port, n->in_ports) {
		std::string name = in_port.first;
		Flow *f = in_port.second;
		os << boost::format("\t\t<inport name=\"%1%\">\n") % name;
		os << "\t\t\t" << FlowSerializer::toString(*f) << "\n";
		os << "\t\t</inport>\n";
	}

	BOOST_FOREACH(ssfp out_port, n->out_ports) {
		std::string name = out_port.first;
		Flow *f = out_port.second;
		os << boost::format("\t\t<outport name=\"%1%\">\n") % name;
		os << "\t\t\t" << FlowSerializer::toString(*f) << "\n";
		os << "\t\t</outport>\n";
	}

	os << "\t</node>\n";
}

void ModelSerializer::deserialize(int time) {
	QString path = QString::fromStdString(pathForTimeStep(time));
	cd3assert(QDir::current().exists(path), "no such state file");
	Deserializer deser(model);
	QXmlSimpleReader reader;
	QFile f(path);
	cd3assert(f.open(QIODevice::ReadOnly), "could not open state file for reading");
	QXmlInputSource s(&f);
	reader.setContentHandler(&deser);
	reader.setErrorHandler(&deser);
	cd3assert(reader.parse(s), "could no load state into  model");
}

Deserializer::Deserializer(IModel *m)
 :	model(m) {
	mapper = IStateSerializer::standard;
	in_state_node = false;
	in_inport = false;
	in_outport = false;
}

Deserializer::~Deserializer() {

}

bool Deserializer::startElement(const QString & namespaceURI,
								const QString & localName,
								const QString & qName,
								const QXmlAttributes & atts ) {
	(void) namespaceURI;
	(void) localName;
	if (qName == "node") {
		std::string name = atts.value("name").toStdString();
		current = model->getNode(name);
		return true;
	}

	if (qName == "state") {
		stateName = atts.value("name").toStdString();
		stateType = atts.value("type").toStdString();
		in_state_node = true;
		return true;
	}
	if (qName == "model")
		return true;

	if (qName == "inport") {
		portName = atts.value("name").toStdString();
		in_inport = true;
		return true;
	}

	if (qName == "outport") {
		portName = atts.value("name").toStdString();
		in_outport = true;
		return true;
	}
	qDebug() << "unknown node " << qName;
	return false;
}

bool Deserializer::endElement(const QString &namespaceURI,
					const QString &localName,
					const QString &qName) {
	(void) namespaceURI;
	(void) localName;
	if (qName == "state") {
		in_state_node = false;
	}

	if (qName == "inport") {
		in_inport = false;
	}

	if (qName == "outport") {
		in_outport = false;
	}

	if (qName == "node") {
		current->pushInStates();
	}
	return true;
}

bool Deserializer::characters(const QString &ch) {
	if (in_state_node) {
		cd3::TypeInfo info;
		info.type_name = stateType;

		cd3assert(mapper.find(info) != mapper.end(),
			   str(format("no serializer for type %1%") % info.type_name));
		mapper[info]->deserialize(boost::algorithm::trim_copy(ch.toStdString()),
								  boost::algorithm::trim_copy(stateName), current);
		return true;
	}
	if (in_inport || in_outport) {
		Flow f = FlowSerializer::fromString(boost::algorithm::trim_copy(ch.toStdString()));
		if (in_inport)
			current->setInPort(portName, &f);
		if (in_outport)
			current->setOutPort(portName, &f);
		return true;
	}
	return true;
}

bool Deserializer::error(const QXmlParseException &exception) {
	qWarning() << "Error on line" << exception.lineNumber()
				<< ", column" << exception.columnNumber() << ":"
				<< exception.message();

	 return false;
}

bool Deserializer::fatalError(const QXmlParseException &exception) {
	qWarning() << "Fatal error on line" << exception.lineNumber()
				<< ", column" << exception.columnNumber() << ":"
				<< exception.message();

	 return false;
}
