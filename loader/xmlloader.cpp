#include "xmlloader.h"

#include <QtXml>
#include <QDataStream>
#include <QDebug>
#include <boost/format.hpp>
#include <iostream>

#include <model.h>
#include <noderegistry.h>
#include <node.h>
#include <simulation.h>
#include <typeregistry.h>

#ifdef DEBUG
#define CONSUME(node) consumed << node
#else
#define CONSUME(node) (void) node;
#endif

void xmlError(QDomNode node, QString error) {
	qDebug() << node.lineNumber() << ":" << node.columnNumber() << "-" << node.nodeName() << error;
}

XmlLoader::XmlLoader(ISimulation *s, IModel *m)
 : model(m), simulation(s) {
	node_registry = new NodeRegistry();
	type_registry = new TypeRegistry();
}

XmlLoader::~XmlLoader() {
	delete type_registry;
	delete node_registry;
}

bool XmlLoader::load(QFile &file) {
	assert(model);
	assert(simulation);

	QString errorStr;
	QDomDocument document("citydrain");
	int errorLine;
	int errorColumn;

	if (!file.open(QIODevice::ReadOnly)) {
		qFatal("could no open file");
		return false;
	}

	if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		std::string s = boost::str(boost::format("%1%") % 1);
		std::cout << s << std::endl;
		return false;
	}


	QStringList paths = loadPluginPaths(document.elementsByTagName("pluginpath"));

	loadNodesFromPlugins(node_registry, paths);
	loadTypesFromPlugins(type_registry, paths);

	loadSimulation(document);
	loadModel(document);


	CONSUME(document.firstChildElement("citydrain").toElement());

#ifdef DEBUG
	checkAllConsumed(document.firstChildElement("citydrain"));
#endif

	return true;
}

void XmlLoader::loadModel(QDomDocument document) {

	loadNodes(document.firstChildElement("citydrain")
			  .firstChildElement("model")
			  .firstChildElement("nodelist"));

	CONSUME(document.firstChildElement("citydrain")
			.firstChildElement("model").toElement());

	model->initNodes(simulation->getSimulationParameters());

	loadConnections(document.firstChildElement("citydrain")
					.firstChildElement("model")
					.firstChildElement("connectionlist"));
}

void XmlLoader::loadSimulation(QDomDocument document) {
	assert(simulation);

	QDomElement time = document.firstChildElement("citydrain")
					   .firstChildElement("simulation")
					   .firstChildElement("time");

	CONSUME(document.firstChildElement("citydrain")
			.firstChildElement("simulation").toElement());

	QDomNamedNodeMap attrs = time.attributes();

	assert(attrs.contains("start"));
	assert(attrs.contains("stop"));
	assert(attrs.contains("dt"));

	CONSUME(time);

	int start = attrs.namedItem("start").nodeValue().toInt();
	int stop = attrs.namedItem("stop").nodeValue().toInt();
	int dt = attrs.namedItem("dt").nodeValue().toInt();

	simulation->setSimulationParameters(SimulationParameters(start, stop, dt));
}

QStringList XmlLoader::loadPluginPaths(QDomNodeList dlist) {
	QStringList plist;	
	for (int i = 0; i < dlist.count(); i++) {
		CONSUME(dlist.at(i).toElement());
		plist.append(dlist.at(i).attributes().namedItem("path").toAttr().value());
	}
	//qDebug() << plist;
	return plist;
}

//TODO globalise
typedef void (*regNodeFunProto)(INodeRegistry *reg);

void XmlLoader::loadNodesFromPlugins(
		INodeRegistry *registry,
		QStringList paths) {

	QString path;
	foreach (path, paths) {
		QLibrary l(path);
		//l.setLoadHints(QLibrary::ExportExternalSymbolsHint);
		l.load();
		regNodeFunProto regFun = (regNodeFunProto) l.resolve("registerNodes");
		if (regFun) {
			regFun(registry);
		} else {
			qWarning() << path << " has no node register hook";
		}
	}
}

typedef void (*regTypeFunProto)(TypeRegistry *reg);

void XmlLoader::loadTypesFromPlugins(
		TypeRegistry *registry,
		QStringList paths) {
	QString path;
	foreach (path, paths) {
		QLibrary l(path);
		//l.setLoadHints(QLibrary::ExportExternalSymbolsHint);
		l.load();
		regTypeFunProto regFun = (regTypeFunProto) l.resolve("registerTypes");
		if (regFun) {
			regFun(registry);
		} else {
			qWarning() << path << " has no type register hook";
		}
	}
}

void XmlLoader::loadNodes(QDomElement element) {
	assert(element.nodeName() == "nodelist");
	CONSUME(element);
	//qDebug() << "start loading nodes";
	QDomNodeList childs = element.childNodes();
	for (int i = 0; i < childs.count(); i++) {
		QDomNode child = childs.at(i);
		assert(child.nodeName() == "node");
		assert(child.isElement());
		loadNode(child.toElement());
	}
}

void XmlLoader::loadNode(QDomElement element) {
	assert(element.nodeName() == "node");
	CONSUME(element);
	std::string id = element.attribute("id", "").toStdString();
	std::string nodeClass = element.attribute("class").toStdString();
	//TODO assert here
	if (!node_registry->doContains(nodeClass)) {
		qDebug() << "no class " << nodeClass.c_str() << " registered";
		return;
	}
	Node *node = node_registry->doCreateNode(nodeClass);

	assert(node);

	QDomNodeList childs = element.childNodes();

	for (int i = 0; i < childs.count(); i++) {
		if (childs.at(i).nodeName() == "parameter") {
			setNodeParameter(node, childs.at(i).toElement());
		}
	}
	model->addNode(id, node);
}

void XmlLoader::loadConnections(QDomElement element) {
	//xmlError(element, element.nodeName());
	//assert(false);
	assert(element.nodeName() == "connectionlist");

	CONSUME(element);

	QDomNodeList childs = element.childNodes();

	for (int i = 0; i < childs.count(); i++) {
		QDomNode child = childs.at(i);

		assert(child.nodeName() == "connection");

		QDomNamedNodeMap source_attrs = child
										.firstChildElement("source")
										.attributes();
		QDomNamedNodeMap sink_attrs = child
									  .firstChildElement("sink")
									  .attributes();

		CONSUME(child.firstChildElement("source"));
		CONSUME(child.firstChildElement("sink"));

		assert(source_attrs.contains("node"));
		assert(source_attrs.contains("port"));
		assert(sink_attrs.contains("node"));
		assert(sink_attrs.contains("port"));

		CONSUME(child.toElement());

		std::string src_node = source_attrs.namedItem("node")
							   .toAttr()
							   .value()
							   .toStdString();
		std::string sin_node = sink_attrs.namedItem("node")
							   .toAttr()
							   .value()
							   .toStdString();

		std::string src_port = source_attrs.namedItem("port")
							   .toAttr()
							   .value()
							   .toStdString();
		std::string sin_port = sink_attrs.namedItem("port")
							   .toAttr()
							   .value()
							   .toStdString();

		model->addConnection(src_node, src_port, sin_node, sin_port);
	}

}

void XmlLoader::setNodeParameter(Node *node, QDomElement element) {
	assert(node);
	assert(element.nodeName() == "parameter");
	CONSUME(element);
	bool simple = element.attribute("kind", "simple") == "simple";

	std::string name = element.attribute("name", "").toStdString();
	std::string type = element.attribute("type", "double").toStdString();

	if (node->const_parameters->find(name) == node->const_parameters->end()) {
		xmlError(element, QString().sprintf("%s no such parameter in node",
											name.c_str()));
	}


	if (simple) {
		QString value = element.attribute("value", "0.0");

		if (type == "double") {
			double d = value.toDouble();
			node->setParameter<double>(name, d);
			return;
		}

		if (type == "int") {
			int ivalue = value.toInt();
			node->setParameter<int>(name, ivalue);
			return;
		}

		if (type == "bool") {
			bool bvalue;
			if (value == "true") {
				bvalue = true;
			} else {
				if (value == "false") {
					xmlError(element, "can not parse bool");
				}
			}
			node->setParameter<bool>(name, bvalue);
			return;
		}

		if (type == "string") {
			std::string std = value.toStdString();
			node->setParameter<std::string>(name, std);
			return;
		}

		xmlError(element, QString().sprintf("%s simple type unknown", type.c_str()));
	} else {
		QDomElement sub = element.firstChild().toElement();
		dont_check << sub;
		type_registry->getByTypeName(type)->setParameter(node, name, sub);
	}
}

#ifdef DEBUG
void XmlLoader::checkAllConsumed(QDomElement root) {
	if (root.isNull()) {
		return;
	}

	if (dont_check.contains(root)) {
		xmlError(root, "ignoring consume");
		return;
	}

	if (!consumed.contains(root)) {
		xmlError(root, "not consumed");
	}

	QDomNodeList childs = root.childNodes();

	for (int i = 0; i < childs.count(); i++) {
		checkAllConsumed(childs.at(i).toElement());
	}
}
#endif
