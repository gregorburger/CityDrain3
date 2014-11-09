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

#include "simulationscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsView>
#include <QDateTime>
#include <QMessageBox>

#include <noderegistry.h>
#include <simulationregistry.h>
#include <simulation.h>
#include <node.h>
#include <nodeconnection.h>
#include <nodeitem.h>
#include <portitem.h>
#include <connectionitem.h>
#include <mapbasedmodel.h>
#include <simulationsaver.h>
#include <guimodelloader.h>
#include <pythonexception.h>

#include "commands/deleteconnection.h"
#include "commands/deletenode.h"
#include "commands/addconnection.h"
#include "commands/addnode.h"
#include "commands/changetime.h"
#include "commands/renamenode.h"

#include <nodeparametersdialog.h>
#include <newsimulationdialog.h>
#include <ui_newsimulationdialog.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace boost::gregorian;
using namespace boost::posix_time;

SimulationScene::SimulationScene(QObject *parent)
	: QGraphicsScene(parent), node_reg(0), sim_reg(0), simulation(0), model(0) {

	if (model_file_name != "") {
		load(model_file_name);
	}
}

SimulationScene::~SimulationScene() {

}

ptime qttopt(const QDateTime &dt) {
	date d(dt.date().year(), dt.date().month(), dt.date().day());
	ptime t(d, time_duration(dt.time().hour(), dt.time().minute(), dt.time().second()));
	return t;
}

QDateTime pttoqt(const boost::posix_time::ptime &pt) {
	date gd = pt.date();
	time_duration gt = pt.time_of_day();
	QDate qdate(gd.year(), gd.month(), gd.day());
	QTime qtime(gt.hours(), gt.minutes(), gt.seconds());

	return QDateTime(qdate, qtime);
}

void SimulationScene::_new() {
	Q_ASSERT(!model);
	Q_ASSERT(!node_reg);
	Q_ASSERT(!sim_reg);
	Q_ASSERT(!simulation);

	sim_reg = new SimulationRegistry();
	NewSimulationDialog ns(sim_reg);
	if (ns.exec()) {
		QDir::setCurrent(ns.ui->projectRootLineEdit->text());
		model = new MapBasedModel();
		node_reg = new NodeRegistry();
		simulation = ns.createSimulation();
		simulation->setModel(model);
		current_connection = 0;
		connection_start = 0;
		Q_EMIT(nodesRegistered());
		Q_EMIT(loaded());
	} else {
		delete sim_reg;
		sim_reg = 0;
	}
}

void SimulationScene::unload() {
	node_items.clear();
	connection_items.clear();
	connections_of_node.clear();
	plugins.clear();
	python_modules.clear();
	python_controller.clear();
	model_file_name = "";
	clear();
	delete sim_reg;
	delete node_reg;
	delete model;
	delete simulation;
	sim_reg = 0;
	node_reg = 0;
	model = 0;
	simulation = 0;
	current_connection = 0;
	connection_start = 0;
	Q_EMIT(unloaded());
}

void SimulationScene::save(QString path) {
	model_file_name = path;
	Q_ASSERT(model_file_name != "");
	SimulationSaver ss(this, model_file_name, plugins, python_modules, python_controller);
	ss.save();
	Q_EMIT(saved());
}

void SimulationScene::load(QString model_file_name) {
	this->model_file_name = model_file_name;
	model = new MapBasedModel();
	node_reg = new NodeRegistry();
	sim_reg = new SimulationRegistry();
	simulation = 0;
	current_connection = 0;
	connection_start = 0;

	Q_ASSERT(model_file_name != "");
	QFile xmlModelFile(model_file_name);
	GuiModelLoader gml(model, node_reg, sim_reg);
	if (!gml.load(xmlModelFile)) {
		QMessageBox::critical(0, "failed to load model file", "Error while loading model file. Refer to Log for more information!");
		return;
	}
	simulation = gml.getSimulation();
	simulation->setModel(model);
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		NodeItem *item = new NodeItem(node);
		item->setPos(gml.getNodePosition(item->getId()));
		add(item);
		if (gml.getFailedNodes().contains(node)) {
			item->changeParameters();
		}
	}

	BOOST_FOREACH(NodeConnection *con, *model->getConnections()) {
		ConnectionItem *citem = new ConnectionItem(this, con);
		add(citem);
	}
	plugins << gml.getPlugins();
	python_modules << gml.getPythonModules();
	python_controller << gml.getPythonController();
	////update();
	Q_EMIT(loaded());
	Q_EMIT(nodesRegistered());
}

//default id is klassname_+counter
std::string SimulationScene::getDefaultId(Node *node) const {
	name_node_map nodes = model->getNamesAndNodes();
	int count = 0;
	std::string id;
	while (true) {
		id = node->getClassName() +  std::string("_") + lexical_cast<std::string>(count++);
		if (nodes.find(id) == nodes.end()) {
			break; //found an id
		}
	}
	return id;
}

void SimulationScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
	if (!simulation) {
		return QGraphicsScene::dropEvent(event);
	}
	event->accept();
	QTreeWidget *treeWidget = static_cast<QTreeWidget*>(event->source());
	std::string klassName = treeWidget->selectedItems()[0]->text(0).toStdString();
	QGraphicsScene::dragMoveEvent(event);

	Node *node = 0;
	NodeItem *nitem = 0;
	try {
		node = node_reg->createNode(klassName);
		std::string id = getDefaultId(node);
		model->addNode(id, node);

		nitem = new NodeItem(node);
		nitem->setPos(event->scenePos());
		this->addItem(nitem);

		if (!nitem->changeParameters(true)) {
			delete nitem;
			model->removeNode(node);
			return;
		}

		add(nitem);
		//update();
		Q_EMIT(changed(new AddNode(this, nitem)));
	} catch (PythonException e) {
		QString msg = QString("Error while initializing Node\n"
							  "python error in log");
		QMessageBox::critical(0, "Python Node init failed", msg);
		if (node) {
			model->removeNode(node);
		}
		if (nitem) {
			delete nitem;
		}
		return;
	}
}

void SimulationScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	if (!simulation) {
		return QGraphicsScene::dragMoveEvent(event);
	}
	event->accept();
}

void SimulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	connection_start = static_cast<PortItem *>(itemAt(event->scenePos()));

	if (connection_start &&
		isOutPort(connection_start) &&
		!connection_start->isConnected()) {

		current_connection = new ConnectionItem(this, connection_start->getNodeItem()->getId(), connection_start->getPortName());
		views()[0]->setDragMode(QGraphicsView::NoDrag);
		return;
	}

	QGraphicsItem *iAt = itemAt(event->scenePos());

	if (iAt && event->button() == Qt::RightButton) {
		QMenu m;
		QAction *del = m.addAction("&delete");
		this->connect(del, SIGNAL(triggered()), SLOT(deleteSelectedItems()));
		m.exec(event->screenPos());
		connection_start = 0;
		return;
	}

	connection_start = 0;
	QGraphicsScene::mousePressEvent(event);
}

void SimulationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	current_mouse = event->scenePos();
	if (connection_start) {
		if (current_connection) {
			current_connection->setSink(event->scenePos());
			update();
		}
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void SimulationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	PortItem *connection_end = static_cast<PortItem *>(itemAt(event->scenePos()));
	views()[0]->setDragMode(QGraphicsView::RubberBandDrag);

	if (connection_end && connection_start &&
		isInPort(connection_end) &&	!connection_end->isConnected() &&
		connection_end != connection_start) {

		Node *start = connection_start->getNodeItem()->getNode();
		Node *end = connection_end->getNodeItem()->getNode();
		std::string out_port = connection_start->getPortName().toStdString();
		std::string in_port = connection_end->getPortName().toStdString();

		NodeConnection *con = simulation->createConnection(start, out_port, end, in_port);
		model->addConnection(con);
		current_connection->setConnection(con);
		add(current_connection);
		connection_start = 0;
		Q_EMIT(changed(new AddConnection(this, current_connection)));
		current_connection = 0;
		update();
		return;
	}

	delete current_connection;
	current_connection = 0;
	connection_start = 0;
	QGraphicsScene::mouseReleaseEvent(event);
}

bool SimulationScene::isInPort(QGraphicsItem *item) const {
	Q_FOREACH(NodeItem *node, node_items) {
		if (node->in_ports.values().contains(static_cast<PortItem*>(item))) {
			return true;
		}
	}
	return false;
}

bool SimulationScene::isOutPort(QGraphicsItem *item) const {
	Q_FOREACH(NodeItem *node, node_items) {
		if (node->out_ports.values().contains(static_cast<PortItem*>(item))) {
			return true;
		}
	}
	return false;
}

void SimulationScene::addPlugin(QString pname) {
	std::string plugin_name = pname.toStdString();
	node_reg->addNativePlugin(plugin_name);
	sim_reg->addNativePlugin(plugin_name); //TODO do we need this?
	plugins << pname;
	Q_EMIT(changed(0));	//do we need undo here?
	Q_EMIT(nodesRegistered());
}

void SimulationScene::addPythonModule(QString pname) {
#ifndef PYTHON_DISABLED
	std::string dir_name = QFileInfo(pname).dir().path().toStdString();
	NodeRegistry::addToPythonPath(dir_name);
	node_reg->addPythonPlugin(pname.toStdString());
	python_modules << pname;
	Q_EMIT(changed(0));	//can't definitly do undo here!
	Q_EMIT(nodesRegistered());
#else
	QMessageBox::critical(0, "python disabled", "Python support is disabled for this build");
#endif
}

void SimulationScene::add(ConnectionItem *item) {
	Q_ASSERT(!connection_items.contains(item));
	connection_items << item;
	Q_ASSERT(!connections_of_node.values(item->getSourceId()).contains(item));
	connections_of_node.insert(item->getSourceId(), item);
	Q_ASSERT(!connections_of_node.values(item->getSinkId()).contains(item));
	connections_of_node.insert(item->getSinkId(), item);
}

void SimulationScene::add(NodeItem *item) {
	Q_ASSERT(!node_items.contains(item->getId()));
	node_items[item->getId()] = item;
	this->connect(item, SIGNAL(changed(QUndoCommand*)), SLOT(nodeChanged(QUndoCommand*)));
	if (!items().contains(item))
		addItem(item);
}

void SimulationScene::remove(ConnectionItem *item) {
	Q_EMIT(changed(new DeleteConnection(this, item)));
}

void SimulationScene::remove(NodeItem *item) {
	QList<ConnectionItem *> items = connections_of_node.values(item->getId());
	Q_FOREACH(ConnectionItem *citem, items) {
		remove(citem);
	}
	Q_EMIT(changed(new DeleteNode(this, item)));
}

void SimulationScene::nodeChanged(QUndoCommand *cmd) {
	Q_EMIT(changed(cmd));
}

void SimulationScene::copy() {
	copied_nodes.clear();
	if (selectedItems().size() == 0)
		return;

	float x = 0.0;
	float y = 0.0;
	int count = 0;

	Q_FOREACH(NodeItem *item, filterNodes(selectedItems())) {
		x += item->pos().x();
		y += item->pos().y();
		count++;
	}

	x /= count;
	y /= count;

	Q_FOREACH(NodeItem *node_item, filterNodes(selectedItems())) {
		CopyState cs;
		cs._class = node_item->getClassName().toStdString();
		cs.parameters = node_item->saveParameters();
		cs.position = QPointF(node_item->pos().x() - x, node_item->pos().y() - y);
		copied_nodes << cs;
	}
}

void SimulationScene::paste() {
	clearSelection();
	Q_FOREACH(CopyState cn, copied_nodes) {
		Node *n = node_reg->createNode(cn._class);
		std::string id  = this->getDefaultId(n);
		model->addNode(id, n);
		NodeItem *item = new NodeItem(n);
		item->setPos(current_mouse.x() + cn.position.x(), current_mouse.y() + cn.position.y());
		item->restoreParameters(cn.parameters);
		SimulationParameters param = simulation->getSimulationParameters();
		n->init(param.start + seconds(param.dt), param.stop, param.dt);
		item->updatePorts();
		item->setSelected(true);
		add(item);
		Q_EMIT(changed(new AddNode(this, item)));
	}
}

void SimulationScene::deleteSelectedItems() {
	QList<NodeItem *> node_items = filterNodes(selectedItems());
	Q_FOREACH(NodeItem *item, node_items) {
		remove(item);
	}

	QList<ConnectionItem *> con_items = filterConnections(selectedItems());

	Q_FOREACH(ConnectionItem *item, con_items) {
		remove(item);
	}
}

bool SimulationScene::setSimulationParameters(SimulationParameters &p) {
	getModel()->deinitNodes();
	if (getModel()->initNodes(p).size() > 0) { //TODO check for uninited nodes here
		return false;
	}
	SimulationParameters before = simulation->getSimulationParameters();
	getSimulation()->setSimulationParameters(p);
	Q_EMIT(simulationParametersChanged());
	Q_EMIT(changed(new ChangeTime(this, before, p)));
	return true;
}

NodeItem *SimulationScene::findItem(QString node_id) const {
	return node_items[node_id];
}

ConnectionItem *SimulationScene::findItem(QString source, QString source_port,
										  QString sink, QString sink_port) const {

	Q_FOREACH(ConnectionItem *item, connection_items) {
		NodeConnection *con = item->getConnection();
		Q_ASSERT(con);
		if (con->source->getId() == source.toStdString() &&
			con->source_port == source_port.toStdString() &&
			con->sink->getId() == sink.toStdString() &&
			con->sink_port == sink_port.toStdString()) {
			return item;
		}
	}
	return 0;
}

void SimulationScene::renameNodeItem(QString old_id, QString new_id) {
	Q_EMIT(changed(new RenameNode(this, old_id, new_id)));
}

void SimulationScene::updateConnections(NodeItem *item) {
	Q_FOREACH(ConnectionItem *citem, connections_of_node.values(item->getId())) {
		citem->updatePositions();
	}
}

QList<NodeItem *> SimulationScene::filterNodes(QList<QGraphicsItem*> items) {
	QList<NodeItem *> filtered;
	Q_FOREACH(QGraphicsItem *item, items) {
		NodeItem *nitem = static_cast<NodeItem *>(item);
		if (node_items.values().contains(nitem))
			filtered << nitem;
	}
	return filtered;
}

QList<ConnectionItem *> SimulationScene::filterConnections(QList<QGraphicsItem*> items) {
	QList<ConnectionItem *> filtered;
	Q_FOREACH(QGraphicsItem *item, items) {
		ConnectionItem *citem = static_cast<ConnectionItem *>(item);
		if (connection_items.contains(citem))
			filtered << citem;
	}
	return filtered;
}
