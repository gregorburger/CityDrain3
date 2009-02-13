#include "xmlloader.h"

#include <QtXml>
#include <QDataStream>
#include <QDebug>
#include <QLibrary>

#include <iostream>

#include <model.h>
#include <noderegistry.h>
#include <node.h>
#include <boost/format.hpp>

#include "typeregistry.h"

void xmlError(QDomNode node, QString error) {
	qDebug() << node.lineNumber() << ":" << node.columnNumber() << error;
}

XmlLoader::XmlLoader(Simulation *s, IModel *m)
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

	loadNodes(document.firstChildElement("citydrain")
			  .firstChildElement("model")
			  .firstChildElement("nodelist"));

	loadConnections(document.firstChildElement("citydrain")
					.firstChildElement("model")
					.firstChildElement("connectionlist"));

	return true;
}

QStringList XmlLoader::loadPluginPaths(QDomNodeList dlist) {
	QStringList plist;	
	for (int i = 0; i < dlist.count(); i++) {
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

	qDebug() << "start loading nodes";
	QDomNodeList childs = element.childNodes();
	for (int i = 0; i < childs.count(); i++) {
		QDomNode child = childs.at(i);
		if (child.nodeName() != "node" || !child.isElement()) {
			xmlError(element, "dom node not a node type");
			continue;
		}

		loadNode(child.toElement());
	}
}

void XmlLoader::loadNode(QDomElement element) {
	assert(element.nodeName() == "node");
	std::string id = element.attribute("id", "").toStdString();
	std::string nodeClass = element.attribute("class").toStdString();
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
	//assert(element.nodeName() == "connectionlist");

	QDomNodeList childs = element.childNodes();

	for (int i = 0; i < childs.count(); i++) {


		QDomNode child = childs.at(i);

		assert(child.nodeName() == "connection");

		QDomNamedNodeMap source_attrs = child.firstChildElement("source").attributes();
		QDomNamedNodeMap sink_attrs = child.firstChildElement("sink").attributes();

		assert(source_attrs.contains("node"));
		assert(source_attrs.contains("port"));
		assert(sink_attrs.contains("node"));
		assert(sink_attrs.contains("port"));

		std::string src_node = source_attrs.namedItem("node").toAttr().value().toStdString();
		std::string sin_node = sink_attrs.namedItem("node").toAttr().value().toStdString();

		std::string src_port = source_attrs.namedItem("port").toAttr().value().toStdString();
		std::string sin_port = sink_attrs.namedItem("port").toAttr().value().toStdString();

		model->addConnection(src_node, src_port, sin_node, sin_port);
	}

}

void XmlLoader::setNodeParameter(Node *node, QDomElement element) {
	assert(node);
	assert(element.nodeName() == "parameter");
	bool simple = element.attribute("kind", "simple") == "simple";

	std::string name = element.attribute("name", "").toStdString();
	std::string type = element.attribute("type", "double").toStdString();

	if (node->const_parameters->find(name) == node->const_parameters->end()) {
		xmlError(element, QString(name.c_str()).append(" no such parameter in node"));
	}


	if (simple) {
		QString value = element.attribute("value", "0.0");

		if (type == "double") {
			node->setState(name, std::auto_ptr<double> (new double(value.toDouble())));
			return;
		}

		if (type == "int") {
			node->setState(name, std::auto_ptr<int> (new int(value.toInt())));
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
			node->setState(name, std::auto_ptr<bool> (new bool(bvalue)));
			return;
		}

		if (type == "string") {
			node->setState(name, std::auto_ptr<std::string> (new std::string(value.toStdString())));
			return;
		}

		xmlError(element, " could not parse simple type parameter");
	} else {
		QDomElement sub = element.firstChild().toElement();
		type_registry->getByTypeName(type)->setParameter(node, name, sub);
	}
}
