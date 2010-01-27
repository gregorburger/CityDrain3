#ifndef GUIMODELLOADER_H
#define GUIMODELLOADER_H

#include "../app/saxloader.h"
#include <QMap>
#include <QPointF>

class GuiModelLoader : public SaxLoader
{
public:
	GuiModelLoader(IModel *m,
				   NodeRegistry *nr = 0,
				   SimulationRegistry *sr = 0);
	bool startElement(const QString &ns,
					  const QString &lname,
					  const QString &qname,
					  const QXmlAttributes &atts);
	QPointF getNodePosition(QString id) const;
	QStringList getPlugins() const { return plugins; }
	QStringList getPythonModules() const { return python_modules; }
private:
	QMap<QString, QPointF> node_positions;
	QStringList plugins, python_modules;
};

#endif // GUIMODELLOADER_H
