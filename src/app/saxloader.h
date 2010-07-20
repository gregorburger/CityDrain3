#ifndef SAXLOADER_H
#define SAXLOADER_H

#include <QXmlDefaultHandler>
#include <cd3globals.h>

struct SaxLoaderPriv;
class IModel;
class Node;
class ISimulation;
class NodeRegistry;
class SimulationRegistry;

class CD3_PUBLIC SaxLoader : public QXmlDefaultHandler
{
public:
	SaxLoader(IModel *m);
	SaxLoader(IModel *m, NodeRegistry *nr, SimulationRegistry *sr);
	~SaxLoader();
	bool startElement(const QString &ns,
					  const QString &lname,
					  const QString &qname,
					  const QXmlAttributes &atts);
	bool endElement(const QString &ns,
					const QString &lname,
					const QString &qname);
	bool error(const QXmlParseException &exception);
	bool fatalError(const QXmlParseException &exception);
	bool warning(const QXmlParseException &exception);
	ISimulation *load(QFile &f);
protected:
	IModel *model;
	ISimulation *simulation;
private:
	SaxLoaderPriv *pd;
	void loadParameter(const QXmlAttributes &atts);
	void breakCycle();
	Node *current;
	bool cycle_break;
	std::string source_id, source_port, sink_port, sink_id;
	bool delete_node_reg;
	bool delete_sim_reg;
};

#endif // SAXLOADER_H
