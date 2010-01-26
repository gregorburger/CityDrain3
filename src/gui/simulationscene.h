#ifndef SIMULATIONSCENE_H
#define SIMULATIONSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <string>
#include <nodeitem.h> //for PortType

class NodeRegistry;
class SimulationRegistry;
class MapBasedModel;
class ISimulation;
class QTreeWidget;
class PortItem;
class ConnectionItem;

class SimulationScene : public QGraphicsScene
{
Q_OBJECT
public:
	SimulationScene(QObject *parent = 0);
	virtual ~SimulationScene();
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	void load(QString path);
	void save(QString path, QStringList plugins, QStringList python_modules);

	NodeRegistry *getNodeRegistry() const { return node_reg; }
	SimulationRegistry *getSimulationRegistry() const { return sim_reg; }
	ISimulation *getSimulation() const { return simulation; }
	void setSimulation(ISimulation *simulation);
	MapBasedModel *getModel() const { return model; }
	QList<NodeItem*> getNodeItems() const { return node_items; }

private Q_SLOTS:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	bool isInPort(QGraphicsItem *item) const;
	bool isOutPort(QGraphicsItem *item) const;
	NodeRegistry *node_reg;
	SimulationRegistry *sim_reg;
	ISimulation *simulation;

	QMap<std::string, int> ids;
	MapBasedModel *model;
	PortItem *connection_start;
	ConnectionItem *current_connection;
	QList<NodeItem*> node_items;
	QList<ConnectionItem *> connection_items;
};

#endif // SIMULATIONSCENE_H
