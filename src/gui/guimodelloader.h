#ifndef GUIMODELLOADER_H
#define GUIMODELLOADER_H

#include "../app/saxloader.h"
#include <QMap>
#include <QSet>
#include <QPointF>
#include <imodel.h>

class GuiModelLoader : public SaxLoader {
public:
	GuiModelLoader(IModel *m,
				   NodeRegistry *nr = 0,
				   SimulationRegistry *sr = 0);
	bool startElement(const QString &ns,
					  const QString &lname,
					  const QString &qname,
					  const QXmlAttributes &atts);
	bool endElement(const QString &ns,
					const QString &lname,
					const QString &qname);
	QPointF getNodePosition(QString id) const;
	QStringList getPlugins() const { return plugins; }
	QStringList getPythonModules() const { return python_modules; }
	QSet<Node *> getFailedNodes() const { return failed_nodes; }
private:
	QMap<QString, QPointF> node_positions;
	QStringList plugins, python_modules;
	QSet<Node *> failed_nodes;
};

#endif // GUIMODELLOADER_H
