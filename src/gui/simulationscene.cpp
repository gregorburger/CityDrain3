#include "simulationscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsView>
#include <QDateTime>

#include <noderegistry.h>
#include <simulationregistry.h>
#include <simulation.h>
#include <node.h>
#include <nodeconnection.h>
#include <module.h>
#include <nodeitem.h>
#include <portitem.h>
#include <connectionitem.h>
#include <mapbasedmodel.h>
#include <simulationsaver.h>
#include <guimodelloader.h>

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

using namespace std;

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
		model = new MapBasedModel();
		node_reg = new NodeRegistry();
		simulation = ns.createSimulation();
		simulation->setModel(model);
		if (ns.ui->defaultNodesCheckBox->isChecked()) {
			addPlugin("nodes");
		}
		current_connection = 0;
		connection_start = 0;
		Q_EMIT(loaded());
	} else {
		delete sim_reg;
		sim_reg = 0;
	}
}

void SimulationScene::unload() {
	node_items.clear();
	connection_items.clear();
	plugins.clear();
	python_modules.clear();
	model_file_name = "";
	clear();
	delete sim_reg;
	delete node_reg;
	delete model;
	if (simulation)
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
	SimulationSaver ss(this, model_file_name, plugins, python_modules);
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
	simulation = gml.load(xmlModelFile);
	simulation->setModel(model);
	QMap<string, NodeItem*> item_map;
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		NodeItem *item = new NodeItem(node);
		node_items << item;
		addItem(item);
		item->setPos(gml.getNodePosition(item->getId()));
		item_map[node->getId()] = item;
		this->connect(item, SIGNAL(changed(QUndoCommand*)), SLOT(nodeChanged(QUndoCommand*)));
		if (gml.getFailedNodes().contains(node)) {
			item->changeParameters();
		}
	}

	BOOST_FOREACH(NodeConnection *con, *model->getConnections()) {
		NodeItem *source = item_map[con->source->getId()];
		PortItem *source_port = source->getOutPort(QString::fromStdString(con->source_port));
		NodeItem *sink = item_map[con->sink->getId()];
		PortItem *sink_port = sink->getInPort(QString::fromStdString(con->sink_port));
		ConnectionItem *citem = new ConnectionItem(source_port, sink_port, 0, 0);
		citem->setConnection(con);
		source_port->setSourceOf(citem);
		sink_port->setSinkOf(citem);
		citem->updatePositions();
		connection_items << citem;
		addItem(citem);
	}
	plugins << gml.getPlugins();
	python_modules << gml.getPythonModules();
	update();
	Q_EMIT(loaded());
	Q_EMIT(nodesRegistered());
}

//default id is klassname_+counter
string SimulationScene::getDefaultId(Node *node) const {
	name_node_map nodes = model->getNamesAndNodes();
	int count = 0;
	string id;
	while (true) {
		id = node->getClassName() +  string("_") + lexical_cast<string>(count++);
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
	QTreeWidget *treeWidget = (QTreeWidget*) event->source();
	string klassName = treeWidget->selectedItems()[0]->text(0).toStdString();
	QGraphicsScene::dragMoveEvent(event);

	Node *node = node_reg->createNode(klassName);
	string id = getDefaultId(node);
	model->addNode(id, node);

	NodeItem *nitem = new NodeItem(node);
	nitem->setPos(event->scenePos());
	this->addItem(nitem);

	if (!nitem->changeParameters(true)) {
		this->removeItem(nitem);
		delete nitem;
		model->removeNode(node);
		return;
	}

	node_items << nitem;

	this->connect(nitem, SIGNAL(changed(QUndoCommand*)), SLOT(nodeChanged(QUndoCommand*)));
	update();
	Q_EMIT(changed(0));
}

void SimulationScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	if (!simulation) {
		return QGraphicsScene::dragMoveEvent(event);
	}
	event->accept();
}

void SimulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	connection_start = (PortItem *) itemAt(event->scenePos());

	if (connection_start &&
		isOutPort(connection_start) &&
		!connection_start->isConnected()) {
		current_connection = new ConnectionItem(connection_start, event->scenePos(), 0, this);
		current_connection->setZValue(0);
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
		current_connection->setSecond(event->scenePos());
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void SimulationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	PortItem *connection_end = (PortItem *) itemAt(event->scenePos());
	views()[0]->setDragMode(QGraphicsView::RubberBandDrag);

	if (connection_end &&
		connection_start &&
		isInPort(connection_end) &&
		!connection_end->isConnected() &&
		connection_end != connection_start) {

		connection_start->setSourceOf(current_connection);
		connection_end->setSinkOf(current_connection);

		Node *start = connection_start->getNodeItem()->getNode();
		Node *end = connection_end->getNodeItem()->getNode();
		string out_port = connection_start->getPortName().toStdString();
		string in_port = connection_end->getPortName().toStdString();

		NodeConnection *con = simulation->createConnection(start, out_port, end, in_port);
		model->addConnection(con);
		current_connection->setConnection(con);
		current_connection->setSink(connection_end);
		connection_items << current_connection;
		connection_start = 0;
		current_connection = 0;
		update();
		Q_EMIT(changed(0));
		return;
	}

	if (current_connection)
		delete current_connection;
	current_connection = 0;
	connection_start = 0;
	QGraphicsScene::mouseReleaseEvent(event);
}

bool SimulationScene::isInPort(QGraphicsItem *item) const {
	Q_FOREACH(NodeItem *node, node_items) {
		if (node->in_ports.values().contains((PortItem*) item)) {
			return true;
		}
	}
	return false;
}

bool SimulationScene::isOutPort(QGraphicsItem *item) const {
	Q_FOREACH(NodeItem *node, node_items) {
		if (node->out_ports.values().contains((PortItem*) item)) {
			return true;
		}
	}
	return false;
}

void SimulationScene::addPlugin(QString pname) {
	string plugin_name = pname.toStdString();
	node_reg->addNativePlugin(plugin_name);
	sim_reg->addNativePlugin(plugin_name); //TODO do we need this?
	plugins << pname;
	Q_EMIT(changed(0));
	Q_EMIT(nodesRegistered());
}

void SimulationScene::addPythonModule(QString pname) {
	QFileInfo module_file(pname);
	PythonEnv::getInstance()->addPythonPath(module_file.dir().absolutePath().toStdString());
	string module_name = module_file.baseName().toStdString();
	PythonEnv::getInstance()->registerNodes(node_reg, module_name);
	python_modules << pname;
	Q_EMIT(changed(0));
	Q_EMIT(nodesRegistered());
}

void SimulationScene::remove(ConnectionItem *item) {
	connection_items.removeAll(item);
	removeItem(item);
	model->removeConnection(item->getConnection());
	delete item;
	Q_EMIT(changed(0));
}

void SimulationScene::remove(NodeItem *item) {
	Q_FOREACH(ConnectionItem *citem, connection_items) {
		if (citem->source && citem->source->getNodeItem() == item) {
			qDebug() << "removed a connection";
			remove(citem);
		}
		if (citem->sink && citem->sink->getNodeItem() == item) {
			qDebug() << "removed a connection";
			remove(citem);
		}
	}

	node_items.removeAll(item);
	removeItem(item);
	model->removeNode(item->getNode());

	delete item;
	Q_EMIT(changed(0));
}

void SimulationScene::nodeChanged(QUndoCommand *cmd) {
	Q_EMIT(changed(cmd));
}

void SimulationScene::copy() {
	copied_nodes.clear();
	if (selectedItems().size() == 0)
		return;

	float x = selectedItems()[0]->pos().x();
	float y = selectedItems()[0]->pos().y();

	Q_FOREACH(QGraphicsItem *item, selectedItems()) {
		NodeItem *node_item = (NodeItem *) item;
		CopyState cs;
		cs._class = node_item->getClassName().toStdString();
		cs.parameters = node_item->saveParameters();
		cs.position = QPointF(x - node_item->pos().x(), y - node_item->pos().y());
		copied_nodes << cs;
	}
}

void SimulationScene::paste() {
	clearSelection();
	Q_FOREACH(CopyState cn, copied_nodes) {
		Node *n = node_reg->createNode(cn._class);
		string id  = this->getDefaultId(n);
		model->addNode(id, n);
		NodeItem *item = new NodeItem(n);
		item->restoreParameters(cn.parameters);
		SimulationParameters param = simulation->getSimulationParameters();
		n->init(param.start, param.stop, param.dt);
		item->updatePorts();
		node_items << item;
		item->setSelected(true);
		this->addItem(item);
		item->setPos(current_mouse.x() + cn.position.x(), current_mouse.y() + cn.position.y());
		this->connect(item, SIGNAL(changed(QUndoCommand*)), SLOT(nodeChanged(QUndoCommand*)));
	}
	if (copied_nodes.size() >  0) {
		Q_EMIT(changed(0));
	}
}

void SimulationScene::deleteSelectedItems() {
	int size = selectedItems().size();
	Q_FOREACH(QGraphicsItem *item, selectedItems()) {
		if (node_items.contains((NodeItem*) item)) {
			remove((NodeItem*) item);
		}
	}

	size = selectedItems().size();

	Q_FOREACH(QGraphicsItem *item, selectedItems()) {
		if (connection_items.contains((ConnectionItem*) item)) {
			remove((ConnectionItem*) item);
		}
	}
	size = selectedItems().size();
}

bool SimulationScene::setSimulationParameters(SimulationParameters &p) {
	getModel()->deinitNodes();
	if (getModel()->initNodes(p).size() > 0) { //TODO check for uninited nodes here
		return false;
	}
	getSimulation()->setSimulationParameters(p);
	Q_EMIT(changed(0));
	return true;
}
