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
	scene->connections_of_node.remove(item->getSourceId(), item);
	scene->connections_of_node.remove(item->getSinkId(), item);
	scene->model->removeConnection(item->getConnection());
	delete item->getConnection();
	delete item;
}

void DeleteConnection::undo() {
	Node *source = scene->getModel()->getNode(this->source);
	Node *sink = scene->getModel()->getNode(this->sink);
	Q_ASSERT(source);
	Q_ASSERT(sink);
	NodeConnection *con = scene->simulation->createConnection(source,
															  source_port,
															  sink,
															  sink_port);
	scene->getModel()->addConnection(con);
	ConnectionItem *item = new ConnectionItem(scene, con);
	scene->add(item);
}

