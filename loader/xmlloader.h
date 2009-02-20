#ifndef XMLLOADER_H
#define XMLLOADER_H

class QFile;
class IModel;
class INodeRegistry;
class TypeRegistry;
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
	XmlLoader(ISimulation *s, IModel *m);
	~XmlLoader();
	bool load(QFile &f);
private:
	void loadModel(QDomDocument document);
	void loadSimulation(QDomDocument document);
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
	ISimulation *simulation;
#ifdef DEBUG
	QList<QDomElement> consumed;
	QList<QDomElement> dont_check;
	void checkAllConsumed(QDomElement root);
#endif
};

#endif // XMLLOADER_H
