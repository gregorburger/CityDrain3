#include "simulationscene.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>

#include "newsimulationdialog.h"

#include <noderegistry.h>
#include <simulationregistry.h>
#include <simulation.h>
#include <node.h>
#include <nodeitem.h>
#include <portitem.h>
#include <mapbasedmodel.h>

#include <boost/lexical_cast.hpp>


using namespace std;

SimulationScene::SimulationScene(QObject *parent)
	: QGraphicsScene(parent) {
	qDebug() << "I'm here";
	model = new MapBasedModel();
	node_reg = new NodeRegistry();
	sim_reg = new SimulationRegistry();
	simulation = 0;
}

SimulationScene::~SimulationScene() {
	delete(model);
}

void SimulationScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
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
	model->addNode(node);
	NodeItem *nitem = new NodeItem(node);
	this->addItem(nitem);
	nitem->setPos(event->scenePos());
	node_items << nitem;
}

void SimulationScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	event->accept();
}

void SimulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem *clickedItem = itemAt(event->scenePos());
	if (!clickedItem) {
		QGraphicsScene::mousePressEvent(event);
		return;
	}

	connection_start = 0;

	Q_FOREACH(NodeItem *item, node_items) {
		if (item->out_ports.contains((PortItem*) clickedItem)) {
			qDebug() << "out port clicked";
			connection_start = (PortItem*) clickedItem;
			break;
		}
	}

	if (!connection_start) {
		QGraphicsScene::mousePressEvent(event);
		return;
	}
}

void SimulationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsScene::mouseMoveEvent(event);
}

void SimulationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsItem *releasedItem = itemAt(event->scenePos());

	if (!releasedItem) {
		QGraphicsScene::mousePressEvent(event);
		return;
	}

	PortItem *connection_end = 0;

	Q_FOREACH(NodeItem *item, node_items) {
		if (item->in_ports.contains((PortItem*) releasedItem)) {
			qDebug() << "in port released";
			connection_end = (PortItem*) releasedItem;
			break;
		}
	}

	if (!connection_end) {
		QGraphicsScene::mouseReleaseEvent(event);
		return;
	}

	qDebug() << "yipee new connection between " << connection_start->getPortName()
			<< " and " << connection_end->getPortName();

	if (!newSimulation()) {
		connection_start = connection_end = 0;
		return;
	}

	Node *start = connection_start->getNodeItem()->getNode();
	Node *end = connection_end->getNodeItem()->getNode();
	string out_port = connection_start->getPortName().toStdString();
	string in_port = connection_end->getPortName().toStdString();

	NodeConnection *con = simulation->createConnection(start, out_port, end, in_port);
	model->addConnection(con);

	connection_start = connection_end = 0;
}

bool SimulationScene::newSimulation() {
	if (simulation)
		return true;
	NewSimulationDialog ns(sim_reg);
	if (ns.exec()) {
		simulation = ns.createSimulation();
		simulation->setModel(model);
		Q_EMIT(simulationCreated());
		return true;
	}

	return false;
}
