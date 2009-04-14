#ifndef XMLLOADER_H
#define XMLLOADER_H


class QFile;
class IModel;
class NodeRegistry;
class TypeRegistry;
class SimulationRegistry;
class Node;
class ISimulation;

#include <QtXml>
#include <QStringList>

#ifdef DEBUG
#include <QList>
#endif


class XmlLoader
{
public:
	XmlLoader(IModel *m);
	~XmlLoader();
	ISimulation *load(QFile &f);
private:
	void loadModel(QDomDocument document);
	void loadSimulation(QDomDocument document);

	void loadNodes(QDomElement element);
	void loadNode(QDomElement element);
	void loadConnections(QDomElement element);

	QStringList loadPluginPaths(QDomNodeList list);
	void loadNodesFromPlugins(NodeRegistry *registry, QStringList paths);
	void loadTypesFromPlugins(TypeRegistry *registry, QStringList paths);
	void loadSimulationsFromPlugins(SimulationRegistry *registry, QStringList paths);

	void setNodeParameter(Node *node, QDomElement element);

private:
	//QDomDocument document;

	NodeRegistry *node_registry;
	TypeRegistry *type_registry;
	SimulationRegistry *sim_registry;

	IModel *model;
	ISimulation *simulation;

#ifdef DEBUG
	QList<QDomElement> consumed;
	QList<QDomElement> dont_check;
	void checkAllConsumed(QDomElement root);
#endif
};

#endif // XMLLOADER_H
