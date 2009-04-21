#ifndef SAXLOADER_H
#define SAXLOADER_H

#include <QXmlDefaultHandler>

struct SaxLoaderPriv;
class IModel;
class Node;
class ISimulation;

class SaxLoader : public QXmlDefaultHandler
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
    ISimulation *load(QFile &f);
private:
    void loadPlugin(QString path);
    void loadParameter(const QXmlAttributes &atts);
    SaxLoaderPriv *pd;
    Node *current;
    std::string source_id, source_port, sink_port, sink_id;
};

#endif // SAXLOADER_H
