/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
	if (lname == "controller") {
		python_controller << atts.value("path");
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
