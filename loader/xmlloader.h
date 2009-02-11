#ifndef XMLLOADER_H
#define XMLLOADER_H

class QFile;
class Model;
class INodeRegistry;
class TypeRegistry;
class Node;

#include <QtXml>
#include <QStringList>


class XmlLoader
{
public:
	XmlLoader(QFile &file);
	~XmlLoader();
	Model *loadModel();
private:
	/*void loadNodes(Model *m);
	void loadConnections(Model *m);*/
	QStringList loadPluginPaths();
	void loadNodesFromPlugins(INodeRegistry *registry, QStringList paths);
	void loadTypesFromPlugins(TypeRegistry *registry, QStringList paths);
	void loadNodes(QDomElement element);
	void loadNode(QDomElement element);
	void loadConnections(QDomElement element);


	void setNodeParameter(Node *node, QDomElement element);

private:
	QDomDocument document;
	QFile &file;
	Model *model;
	INodeRegistry *node_registry;
	TypeRegistry *type_registry;
};

#endif // XMLLOADER_H
