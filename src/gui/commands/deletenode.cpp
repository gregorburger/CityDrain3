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

	nitem->setPos(pos);
	scene->addItem(nitem);

	scene->node_items << nitem;
	scene->connect(nitem, SIGNAL(changed(QUndoCommand*)), SLOT(nodeChanged(QUndoCommand*)));
	scene->update();
}

void DeleteNode::redo() {
	NodeItem *item = scene->findItem(QString::fromStdString(node_id));
	Q_ASSERT(item != 0);

	scene->node_items.removeAll(item);
	scene->removeItem(item);
	scene->model->removeNode(item->getNode());

	delete item;
}
