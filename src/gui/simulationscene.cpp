#include "simulationscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

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
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;

SimulationScene::SimulationScene(QString model_file_name, QObject *parent)
	: QGraphicsScene(parent), model_file_name(model_file_name) {
	model = new MapBasedModel();
	node_reg = new NodeRegistry();
	sim_reg = new SimulationRegistry();
	simulation = 0;
	current_connection = 0;
	connection_start = 0;
	if (model_file_name != "") {
		load();
	}
}

SimulationScene::~SimulationScene() {
	delete sim_reg;
	delete node_reg;
	delete model;
	if (simulation)
		delete simulation;
}

void SimulationScene::save() {
	Q_ASSERT(model_file_name != "");
	SimulationSaver ss(this, model_file_name, plugins, python_modules);
	ss.save();
	Q_EMIT(unsavedChanged(false));
}

void SimulationScene::load() {
	Q_ASSERT(model->empty());
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
		this->connect(item, SIGNAL(changed(NodeItem*)), SLOT(nodeChanged(NodeItem*)));
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
	Q_EMIT(unsavedChanged(false));
}

void SimulationScene::setSimulation(ISimulation *simulation) {
	simulation->setModel(model);
	this->simulation = simulation;
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
	this->addItem(nitem);

	if (!nitem->changeParameters()) {
		this->removeItem(nitem);
		delete nitem;
		model->removeNode(node);
		return;
	}

	nitem->setPos(event->scenePos());
	node_items << nitem;

	this->connect(nitem, SIGNAL(changed(NodeItem*)), SLOT(nodeChanged(NodeItem*)));
	update();
	Q_EMIT(unsavedChanged(true));
}

void SimulationScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	if (!simulation) {
		return QGraphicsScene::dragMoveEvent(event);
	}
	event->accept();
}

void SimulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	connection_start = (PortItem *) itemAt(event->scenePos());

	if (connection_start && isOutPort(connection_start) && !connection_start->isConnected()) {
		current_connection = new ConnectionItem(connection_start, event->scenePos(), 0, this);
		current_connection->setZValue(0);
		return;
	}

	QGraphicsItem *iAt = itemAt(event->scenePos());

	if (iAt && event->button() == Qt::RightButton &&
		connection_items.contains((ConnectionItem*)iAt)) {
		QMenu m;
		QAction *del = m.addAction("&delete");
		QAction *selected = m.exec(event->screenPos());
		if (selected == del) {
			remove((ConnectionItem *) iAt);
		}
	}

	if (iAt && event->button() == Qt::RightButton &&
		node_items.contains((NodeItem*)iAt)) {
		QMenu m;
		QAction *del = m.addAction("&delete");
		QAction *selected = m.exec(event->screenPos());
		if (selected == del) {
			remove((NodeItem *) iAt);
		}
	}

	connection_start = 0;
	QGraphicsScene::mousePressEvent(event);
}

void SimulationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (connection_start) {
		current_connection->setSecond(event->scenePos());
	}
	QGraphicsScene::mouseMoveEvent(event);
	return;
}

void SimulationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	PortItem *connection_end = (PortItem *) itemAt(event->scenePos());

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
		Q_EMIT(unsavedChanged(true));
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
	Q_EMIT(unsavedChanged(true));
}

void SimulationScene::addPythonModule(QString pname) {
	QFileInfo module_file(pname);
	PythonEnv::getInstance()->addPythonPath(module_file.dir().absolutePath().toStdString());
	string module_name = module_file.baseName().toStdString();
	PythonEnv::getInstance()->registerNodes(node_reg, module_name);
	python_modules << pname;
	Q_EMIT(unsavedChanged(true));
}

void SimulationScene::remove(ConnectionItem *item) {
	connection_items.removeAll(item);
	removeItem(item);
	model->removeConnection(item->getConnection());
	delete item;
	Q_EMIT(unsavedChanged(true));
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
	Q_EMIT(unsavedChanged(true));
}

void SimulationScene::nodeChanged(NodeItem *nitem) {
	(void) nitem;
	Q_EMIT(unsavedChanged(true));
}

void SimulationScene::copy() {
	copied_nodes.clear();
	Q_FOREACH(QGraphicsItem *item, selectedItems()) {
		NodeItem *node_item = (NodeItem *) item;
		copied_nodes << copied_node(node_item->getClassName().toStdString(), node_item->saveParameters());
	}
}

void SimulationScene::paste() {
	Q_FOREACH(copied_node cn, copied_nodes) {
		Node *n = node_reg->createNode(cn.first);
		string id  = this->getDefaultId(n);
		model->addNode(id, n);
		NodeItem *item = new NodeItem(n);
		item->restoreParameters(cn.second);
		node_items << item;
		this->addItem(item);
	}
	if (copied_nodes.size() >  0) {
		Q_EMIT(unsavedChanged(true));
	}
}
