#include "deletenode.h"
#include <nodeitem.h>
#include <node.h>
#include <simulationscene.h>
#include <mapbasedmodel.h>
#include <noderegistry.h>

DeleteNode::DeleteNode(SimulationScene *scene, NodeItem *item)
	: scene(scene), node_id(item->getNode()->getId()),
	node_class(item->getNode()->getClassName()),
	parameters(item->saveParameters()),
	pos(item->pos()) {

}

void DeleteNode::undo() {
	Node *node = scene->node_reg->createNode(node_class);
	scene->model->addNode(node_id, node);

	NodeItem *nitem = new NodeItem(node);

	nitem->restoreParameters(parameters);
	SimulationParameters sp = scene->simulation->getSimulationParameters();
	node->init(sp.start + seconds(sp.dt), sp.stop, sp.dt);
	nitem->updatePorts();
	nitem->setPos(pos);
	scene->add(nitem);
	//scene->update();
}

void DeleteNode::redo() {
	NodeItem *item = scene->findItem(QString::fromStdString(node_id));
	Q_ASSERT(item != 0);
	scene->disconnect(item);
	scene->node_items.remove(item->getId());
	Node *n = item->getNode();
	delete item;
	scene->model->removeNode(n); //deletes n
}
