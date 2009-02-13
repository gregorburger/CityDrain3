#ifndef XMLLOADER_H
#define XMLLOADER_H

class QFile;
class IModel;
class INodeRegistry;
class TypeRegistry;
class Node;
class Simulation;

#include <QtXml>
#include <QStringList>


class XmlLoader
{
public:
	XmlLoader(Simulation *s, IModel *m);
	~XmlLoader();
	bool load(QFile &f);
private:
	void loadModel();
	void loadSimulation();
	void loadNodesFromPlugins(INodeRegistry *registry, QStringList paths);
	void loadTypesFromPlugins(TypeRegistry *registry, QStringList paths);
	void loadNodes(QDomElement element);
	void loadNode(QDomElement element);
	void loadConnections(QDomElement element);

	QStringList loadPluginPaths(QDomNodeList list);


	void setNodeParameter(Node *node, QDomElement element);

private:
	//QDomDocument document;

	INodeRegistry *node_registry;
	TypeRegistry *type_registry;

	IModel *model;
	Simulation *simulation;
};

#endif // XMLLOADER_H
