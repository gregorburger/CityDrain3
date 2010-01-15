#include "simulationscene.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QTreeWidget>

#include <noderegistry.h>
#include <node.h>
#include <nodeitem.h>
#include <portitem.h>
#include <mapbasedmodel.h>

#include <boost/lexical_cast.hpp>


using namespace std;

SimulationScene::SimulationScene(NodeRegistry *reg, QObject *parent)
	: QGraphicsScene(parent), reg(reg), model(new MapBasedModel()) {
	qDebug() << "I'm here";
	connect(this, SIGNAL(selectionChanged()), SLOT(on_selectionChanged()));
}

SimulationScene::~SimulationScene() {
	delete(model);
}

void SimulationScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
	event->accept();
	QTreeWidget *treeWidget = (QTreeWidget*) event->source();
	string klassName = treeWidget->selectedItems()[0]->text(0).toStdString();
	QGraphicsScene::dragMoveEvent(event);

	Node *node = reg->createNode(klassName);
	//default id is klassname_+counter
	if (!ids.contains(klassName))
		ids[klassName] = 0;
	string id_count = lexical_cast<string>(ids[node->getClassName()]++);
	node->setId(node->getClassName() + string("_") + id_count);
	model->addNode(node);
	NodeItem *nitem = new NodeItem(node);
	this->addItem(nitem);
	nitem->setPos(event->scenePos());
	nitem->connectItems(this);
}

void SimulationScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
	event->accept();
}

void SimulationScene::on_selectionChanged() {
	qDebug() << "selected";
}

void SimulationScene::connectionStart(PortItem *source) {
	qDebug() << "SimulationScene::connectionStart();";
	conStart = source;
}

void SimulationScene::connectionEnd(PortItem *source) {
	qDebug() << "SimulationScene::connectionEnd();";
	if (conStart && source != conStart) {
		qDebug() << "adding connection";
	}
}
