#ifndef SAXLOADER_H
#define SAXLOADER_H

#include <QXmlDefaultHandler>
#include <cd3globals.h>

struct SaxLoaderPriv;
class IModel;
class Node;
class ISimulation;

class CD3_PUBLIC SaxLoader : public QXmlDefaultHandler
{
public:
	SaxLoader(IModel *m);
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
private:
	void loadParameter(const QXmlAttributes &atts);
	SaxLoaderPriv *pd;
	Node *current;
	std::string source_id, source_port, sink_port, sink_id;
};

#endif // SAXLOADER_H
