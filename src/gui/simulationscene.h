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

class SimulationScene : public QGraphicsScene
{
Q_OBJECT
public:
	SimulationScene(QObject *parent = 0);
	virtual ~SimulationScene();
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);

	bool newSimulation();

	NodeRegistry *getNodeRegistry() const { return node_reg; }
	SimulationRegistry *getSimulationRegistry() const { return sim_reg; }
	ISimulation *getSimulation() const { return simulation; }

Q_SIGNALS:
	void simulationCreated();

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
	QGraphicsLineItem *current_connection;
	QList<NodeItem*> node_items;
};

#endif // SIMULATIONSCENE_H
