#include "guimodelloader.h"
#include <imodel.h>

GuiModelLoader::GuiModelLoader(IModel *m,
							   NodeRegistry *nr,
							   SimulationRegistry *sr) : SaxLoader(m, nr, sr) {
}

QPointF GuiModelLoader::getNodePosition(QString id) const {
	return node_positions[id];
}

bool GuiModelLoader::startElement(const QString &ns,
								  const QString &lname,
								  const QString &qname,
								  const QXmlAttributes &atts) {
	if (lname != "nodeposition") {
		return SaxLoader::startElement(ns, lname, qname, atts);;
	}

	QString sid = atts.value("id");
	QString sx = atts.value("x");
	QString sy = atts.value("y");

	Q_ASSERT(sid != "");
	Q_ASSERT(sx != "");
	Q_ASSERT(sy != "");

	node_positions[sid] = QPointF(sx.toDouble(), sy.toDouble());
	return SaxLoader::startElement(ns, lname, qname, atts);
}
