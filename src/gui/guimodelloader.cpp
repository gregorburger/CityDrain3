#include "guimodelloader.h"
#include <imodel.h>
#include <simulation.h>
#include <boost/foreach.hpp>

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
	if (lname == "nodeposition") {
		QString sid = atts.value("id");
		QString sx = atts.value("x");
		QString sy = atts.value("y");

		Q_ASSERT(sid != "");
		Q_ASSERT(sx != "");
		Q_ASSERT(sy != "");

		node_positions[sid] = QPointF(sx.toDouble(), sy.toDouble());
	}

	if (lname == "pluginpath") {
		plugins << atts.value("path");
	}
	if (lname == "pythonmodule") {
		python_modules << atts.value("module");
	}

	return SaxLoader::startElement(ns, lname, qname, atts);
}

bool GuiModelLoader::endElement(const QString &ns,
								const QString &lname,
								const QString &qname) {
	if (lname == "nodelist") {
		node_set_type ns = model->initNodes(simulation->getSimulationParameters());
		BOOST_FOREACH(Node *n, ns) {
			failed_nodes << n;
		}

		return true;
	}
	return SaxLoader::endElement(ns, lname, qname);
}
