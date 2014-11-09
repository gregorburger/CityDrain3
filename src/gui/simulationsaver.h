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

#ifndef SIMULATIONSAVER_H
#define SIMULATIONSAVER_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QStringList>

class SimulationScene;
class NodeItem;
class ISimulation;
class IModel;
class Node;
class NodeParameter;
class QXmlStreamWriter;
class QFile;

class SimulationSaver : public QObject
{
Q_OBJECT
public:
	SimulationSaver(SimulationScene *scene,
					QString path,
					QStringList plugins,
					QStringList python_modules,
					QStringList python_controller);
	void setPath(QString path) { this->path = path; }
	QString getPath() const { return this->path; }
	virtual ~SimulationSaver();

public Q_SLOTS:
	void save();

private:
	void saveSimulation(ISimulation *sim);
	void saveModel(IModel *model);
	void saveFlowDefinition();
	void saveNodeParameters(const Node *n);
	void saveFlowParameter(NodeParameter *p);
	void saveNodePositions(QList<NodeItem*> items);
private:
	SimulationScene *scene;
	QString path;
	QStringList plugins, python_modules, python_controller;
	QXmlStreamWriter *writer;
	QFile *out;
};

#endif // SIMULATIONSAVER_H
