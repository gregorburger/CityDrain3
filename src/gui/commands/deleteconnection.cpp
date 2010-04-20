#include "deleteconnection.h"
#include "../connectionitem.h"
#include <nodeconnection.h>
#include <node.h>
#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <portitem.h>

DeleteConnection::DeleteConnection(SimulationScene *scene, ConnectionItem *item)
	: scene(scene),
	source(item->getConnection()->source->getId()),
	source_port(item->getConnection()->source_port),
	sink(item->getConnection()->sink->getId()),
	sink_port(item->getConnection()->sink_port) {
}

void DeleteConnection::redo() {
	ConnectionItem *item = scene->findItem(QString::fromStdString(source), QString::fromStdString(source_port),
										   QString::fromStdString(sink), QString::fromStdString(sink_port));
	Q_ASSERT(item != 0);
	scene->connection_items.removeAll(item);
	scene->removeItem(item);
	scene->model->removeConnection(item->getConnection());
	delete item;
}


void DeleteConnection::undo() {
	NodeItem *source_item = scene->findItem(QString::fromStdString(source));
	NodeItem *sink_item = scene->findItem(QString::fromStdString(sink));
	Q_ASSERT(source_item != 0 && sink_item != 0);
	PortItem * source_port_item = source_item->getOutPort(QString::fromStdString(source_port));
	PortItem * sink_port_item = sink_item->getInPort(QString::fromStdString(sink_port));
	Q_ASSERT(source_port_item != 0 && sink_port_item != 0);
	ConnectionItem *item = new ConnectionItem(source_port_item, sink_port_item, 0, 0);
	NodeConnection *con = scene->simulation->createConnection(source_item->getNode(),
															  source_port,
															  sink_item->getNode(),
															  sink_port);
	item->setConnection(con);
	source_port_item->setSourceOf(item);
	sink_port_item->setSinkOf(item);
	item->updatePositions();
	scene->connection_items << item;
	scene->addItem(item);
}

