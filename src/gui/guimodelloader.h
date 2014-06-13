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
	QStringList getPythonController() const { return python_controller; }
	QSet<Node *> getFailedNodes() const { return failed_nodes; }
private:
	QMap<QString, QPointF> node_positions;
	QStringList plugins, python_modules, python_controller;
	QSet<Node *> failed_nodes;
};

#endif // GUIMODELLOADER_H
