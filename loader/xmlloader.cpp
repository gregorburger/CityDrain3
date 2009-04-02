#include "xmlloader.h"

#include <QtXml>
#include <QDataStream>
#include <QDebug>
#include <boost/format.hpp>
#include <iostream>

using namespace boost;

#include <node.h>
#include <simulation.h>
#include <model.h>

#include <noderegistry.h>
#include <simulationregistry.h>
#include <typeregistry.h>


#include <cd3assert.h>

#ifdef DEBUG
#define CONSUME(node) consumed << node
#else
#define CONSUME(node) (void) node;
#endif

void xmlError(QDomNode node, QString error) {
	qDebug() << node.lineNumber() << ":" << node.columnNumber() << "-" << node.nodeName() << error;
}

XmlLoader::XmlLoader(IModel *m)
 : model(m) {
	node_registry = new NodeRegistry();
	type_registry = new TypeRegistry();
	sim_registry = new SimulationRegistry();
	simulation = 0;
}

XmlLoader::~XmlLoader() {
	delete type_registry;
	delete node_registry;
	delete sim_registry;
}

ISimulation *XmlLoader::load(QFile &file) {
	cd3assert(model, "model null");

	QString errorStr;
	QDomDocument document("citydrain");
	int errorLine;
	int errorColumn;

	if (!file.open(QIODevice::ReadOnly)) {
		qFatal("could no open file");
		return 0;
	}

	if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		std::cerr << "xml error[" << errorLine << ":" << errorColumn << errorStr.toStdString() << std::endl;
		return 0;
	}


	QStringList paths = loadPluginPaths(document.elementsByTagName("pluginpath"));

	loadNodesFromPlugins(node_registry, paths);
	loadTypesFromPlugins(type_registry, paths);
	loadSimulationsFromPlugins(sim_registry, paths);

	loadSimulation(document);
	loadModel(document);


	CONSUME(document.firstChildElement("citydrain").toElement());

#ifdef DEBUG
	checkAllConsumed(document.firstChildElement("citydrain"));
#endif

	return simulation;
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
	//cd3assert(simulation, "simulation null");

	QDomElement simElem = document.firstChildElement("citydrain")
					   .firstChildElement("simulation");

	cd3assert(simElem.hasAttribute("class"), "sim element has no class attribute");

	std::string simclass = simElem.attribute("class").toStdString();
	cd3assert(simclass != "", "sim class may not be empty");
	cd3assert(sim_registry->contains(simclass), "no such simulation class registered");

	simulation = sim_registry->createSimulation(simclass);

	QDomElement time = document.firstChildElement("citydrain")
					   .firstChildElement("simulation")
					   .firstChildElement("time");

	CONSUME(document.firstChildElement("citydrain")
			.firstChildElement("simulation").toElement());

	QDomNamedNodeMap attrs = time.attributes();

	cd3assert(attrs.contains("start"), "no start attribute found");
	cd3assert(attrs.contains("stop"), "no stop attribute found");
	cd3assert(attrs.contains("dt"), "no dt attribute found");

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
typedef void (*regNodeFunProto)(NodeRegistry *reg);

void XmlLoader::loadNodesFromPlugins(
		NodeRegistry *registry,
		QStringList paths) {

	QString path;
	Q_FOREACH (path, paths) {
		cd3assert(QFile::exists(path), str(format("could not find plugin %1%") % path.toStdString()));
		QLibrary l(path);
		//l.setLoadHints(QLibrary::ExportExternalSymbolsHint);
		cd3assert(l.load(), str(format("could not load plugin: %1%") % path.toStdString()));
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
	Q_FOREACH (path, paths) {
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

typedef void (*regSimFunProto)(SimulationRegistry *reg);

void XmlLoader::loadSimulationsFromPlugins(
		SimulationRegistry *registry,
		QStringList paths) {

	QString path;
	Q_FOREACH (path, paths) {
		QLibrary l(path);
		//l.setLoadHints(QLibrary::ExportExternalSymbolsHint);
		l.load();
		regSimFunProto regFun = (regSimFunProto) l.resolve("registerSimulations");
		if (regFun) {
			regFun(registry);
		} else {
			qWarning() << path << " has no node register hook";
		}
	}
}

void XmlLoader::loadNodes(QDomElement element) {
	cd3assert(element.nodeName() == "nodelist", "no nodelist element found");
	CONSUME(element);
	//qDebug() << "start loading nodes";
	QDomNodeList childs = element.childNodes();
	for (int i = 0; i < childs.count(); i++) {
		QDomNode child = childs.at(i);
		cd3assert(child.nodeName() == "node", "no child node \"node\" found");
		cd3assert(child.isElement(), "not an element");
		loadNode(child.toElement());
	}
}

void XmlLoader::loadNode(QDomElement element) {
	cd3assert(element.nodeName() == "node", "not a \"node\" element");
	CONSUME(element);
	std::string id = element.attribute("id", "").toStdString();
	std::string nodeClass = element.attribute("class").toStdString();
	std::string script = element.attribute("script", "").toStdString();
	//TODO assert here
	cd3assert(node_registry->contains(nodeClass), "no such Node class registered");
	Node *node = script == "" ? node_registry->createNode(nodeClass) : node_registry->createNode(nodeClass, script);

	cd3assert(node, "node is null");

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
	//cd3assert(false);
	cd3assert(element.nodeName() == "connectionlist", "not a connectionlist element");

	CONSUME(element);

	QDomNodeList childs = element.childNodes();

	for (int i = 0; i < childs.count(); i++) {
		QDomNode child = childs.at(i);

		cd3assert(child.nodeName() == "connection", "not a connectionlist element");

		QDomNamedNodeMap source_attrs = child
										.firstChildElement("source")
										.attributes();
		QDomNamedNodeMap sink_attrs = child
									  .firstChildElement("sink")
									  .attributes();

		CONSUME(child.firstChildElement("source"));
		CONSUME(child.firstChildElement("sink"));

		cd3assert(source_attrs.contains("node"), "connection has no source node");
		cd3assert(source_attrs.contains("port"), "connection has no source port");
		cd3assert(sink_attrs.contains("node"), "connection has no sink node");
		cd3assert(sink_attrs.contains("port"), "connection has no sink port");

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
	cd3assert(node, "can not set null node parameters");
	cd3assert(element.nodeName() == "parameter", "no parameter node");
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

		if (type == "float") {
			float d = value.toFloat();
			node->setParameter<float>(name, d);
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
