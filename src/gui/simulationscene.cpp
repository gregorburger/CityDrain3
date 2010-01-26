#include "simulationscene.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>
#include <QMenu>
#include <QDebug>

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

#include <nodeparametersdialog.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;

SimulationScene::SimulationScene(QObject *parent)
	: QGraphicsScene(parent) {
	model = new MapBasedModel();
	node_reg = new NodeRegistry();
	sim_reg = new SimulationRegistry();
	simulation = 0;
	current_connection = 0;
	connection_start = 0;
}

SimulationScene::~SimulationScene() {
	delete(model);
}

void SimulationScene::save(QString path, QStringList plugins, QStringList python_modules) {
	SimulationSaver ss(this, path, plugins, python_modules);
	ss.save();
}

void SimulationScene::load(QString path) {
	Q_ASSERT(model->empty());
	QFile xmlModelFile(path);
	GuiModelLoader gml(model, node_reg, sim_reg);
	simulation = gml.load(xmlModelFile);
	QMap<string, NodeItem*> item_map;
	BOOST_FOREACH(Node *node, *model->getNodes()) {
		NodeItem *item = new NodeItem(node);
		node_items << item;
		addItem(item);
		item->setPos(gml.getNodePosition(item->getId()));
		item_map[node->getId()] = item;
	}

	BOOST_FOREACH(NodeConnection *con, *model->getConnections()) {
		NodeItem *source = item_map[con->source->getId()];
		PortItem *source_port = source->getOutPort(QString::fromStdString(con->source_port));
		NodeItem *sink = item_map[con->sink->getId()];
		PortItem *sink_port = sink->getInPort(QString::fromStdString(con->sink_port));
		ConnectionItem *citem = new ConnectionItem(source_port, sink_port, 0, 0);
		source_port->setSourceOf(citem);
		sink_port->setSinkOf(citem);
		citem->updatePositions();
		connection_items << citem;
		addItem(citem);
	}
	update();
}

void SimulationScene::setSimulation(ISimulation *simulation) {
	simulation->setModel(model);
	this->simulation = simulation;
}

void SimulationScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	NodeItem *item = (NodeItem *) itemAt(event->scenePos());
	if (item && node_items.contains(item)) {

		NodeParametersDialog np(item->getNode());
		if (np.exec()) {
			item->getNode()->deinit();
			np.updateNodeParameters();
			SimulationParameters sp = simulation->getSimulationParameters();
			item->getNode()->init(sp.start, sp.stop, sp.dt);
			item->nodeChanged();
			item->update();
		}
		return;
	}

	QGraphicsScene::mouseDoubleClickEvent(event);
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

	//default id is klassname_+counter
	if (!ids.contains(klassName))
		ids[klassName] = 0;
	string id_count = lexical_cast<string>(ids[node->getClassName()]++);
	node->setId(node->getClassName() + string("_") + id_count);

	SimulationParameters sp = simulation->getSimulationParameters();

	do {
		if (node->getParameters().size() > 0) {
			NodeParametersDialog np(node);
			if (np.exec()) {
				np.updateNodeParameters();
			} else {
				delete node;
				return;
			}
		}
	} while (!node->init(sp.start, sp.stop, sp.dt));
	model->addNode(node);
	NodeItem *nitem = new NodeItem(node);
	this->addItem(nitem);
	nitem->setPos(event->scenePos());
	node_items << nitem;
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
		qDebug() << "connection right selected";
	}

	if (iAt && event->button() == Qt::RightButton &&
		node_items.contains((NodeItem*)iAt)) {
		QMenu m;
		QAction *del = m.addAction("&delete");
		QAction *selected = m.exec(event->screenPos());
		NodeItem *nitem = (NodeItem *) iAt;
		if (selected == del) {
			removeItem(iAt);
			model->removeNode(nitem->getNode());
			delete nitem;
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

	if (connection_end && isInPort(connection_end) && !connection_end->isConnected()) {

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
		if (node->in_ports.contains((PortItem*) item)) {
			return true;
		}
	}
	return false;
}

bool SimulationScene::isOutPort(QGraphicsItem *item) const {
	Q_FOREACH(NodeItem *node, node_items) {
		if (node->out_ports.contains((PortItem*) item)) {
			return true;
		}
	}
	return false;
}
