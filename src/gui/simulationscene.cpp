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

uint qHash(std::string s) {
	return qHash(QString::fromStdString(s));
}

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
}

void SimulationScene::setSimulation(ISimulation *simulation) {
	simulation->setModel(model);
	this->simulation = simulation;
}



void SimulationScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	NodeItem *item = (NodeItem *) itemAt(event->scenePos());
	if (item && node_items.contains(item)) {
		QMap<string, PortItem*> in_before = item->in_ports;
		QMap<string, PortItem*> out_before = item->out_ports;

		NodeParametersDialog np(item->getNode());
		if (np.exec()) {
			item->getNode()->deinit();
			np.updateNodeParameters();
			SimulationParameters sp = simulation->getSimulationParameters();
			item->getNode()->init(sp.start, sp.stop, sp.dt);

			QMap<string, Flow*> in_after(*item->getNode()->const_in_ports);
			QMap<string, Flow*> out_after(*item->getNode()->const_out_ports);

			QSet<string> in_removed = in_before.keys().toSet() - in_after.keys().toSet();

			Q_FOREACH(string s, in_removed) {
				PortItem *pitem = in_before[s];
				if (pitem->getSinkOf()) {
					remove(pitem->getSinkOf());
				}
				item->in_ports.remove(s);
				delete pitem;
			}

			QSet<string> out_removed = out_before.keys().toSet() - out_after.keys().toSet();

			Q_FOREACH(string s, out_removed) {
				PortItem *pitem = out_before[s];
				if (pitem->getSourceOf()) {
					remove(pitem->getSourceOf());
				}
				item->out_ports.remove(s);
				delete pitem;
			}

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
}

void SimulationScene::addPythonModule(QString pname) {
	QFileInfo module_file(pname);
	PythonEnv::getInstance()->addPythonPath(module_file.dir().absolutePath().toStdString());
	string module_name = module_file.baseName().toStdString();
	PythonEnv::getInstance()->registerNodes(node_reg, module_name);
	python_modules << pname;
}

void SimulationScene::remove(ConnectionItem *item) {
	connection_items.removeAll(item);
	removeItem(item);
	model->removeConnection(item->getConnection());
	delete item;
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
}

