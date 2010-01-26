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
private:
	QMap<QString, QPointF> node_positions;
};

#endif // GUIMODELLOADER_H
