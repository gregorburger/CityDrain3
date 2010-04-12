#ifndef SIMULATIONSCENE_H
#define SIMULATIONSCENE_H

#include <string>

#include <QGraphicsScene>
#include <QMap>
#include <nodeitem.h> //for PortType
#include <boost/date_time/posix_time/posix_time.hpp>

class NodeRegistry;
class SimulationRegistry;
class MapBasedModel;
class ISimulation;
class QTreeWidget;
class PortItem;
class ConnectionItem;

typedef std::pair<std::string, QMap<std::string, std::string> > copied_node;

class SimulationScene : public QGraphicsScene
{
Q_OBJECT
public:
	SimulationScene(QObject *parent = 0);
	virtual ~SimulationScene();
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


	const NodeRegistry *getNodeRegistry() const { return node_reg; }
	SimulationRegistry *getSimulationRegistry() const { return sim_reg; }
	ISimulation *getSimulation() const { return simulation; }
	//void setSimulation(ISimulation *simulation);
	MapBasedModel *getModel() const { return model; }
	QList<NodeItem*> getNodeItems() const { return node_items; }

	QString getModelFileName() const { return model_file_name; }
	//void setModelFileName(QString name) { model_file_name = name; }
	void addPlugin(QString pname);
	void addPythonModule(QString pname);

	void remove(NodeItem *item);
	void remove(ConnectionItem *item);

public Q_SLOTS:
	void _new();
	void save(QString path);
	void load(QString path);
	void unload();
	void copy();
	void paste();

Q_SIGNALS:
	void changed();
	void loaded();
	void unloaded();
	void saved();
	void nodesRegistered();

private Q_SLOTS:
	void nodeChanged(NodeItem *nitem);

private:
	std::string getDefaultId(Node *node) const;
	bool isInPort(QGraphicsItem *item) const;
	bool isOutPort(QGraphicsItem *item) const;
	NodeRegistry *node_reg;
	SimulationRegistry *sim_reg;
	ISimulation *simulation;

	MapBasedModel *model;
	PortItem *connection_start;
	ConnectionItem *current_connection;
	QList<NodeItem*> node_items;
	QList<ConnectionItem *> connection_items;
	QString model_file_name;
	QStringList plugins, python_modules;
	bool unsaved;
	QList<copied_node> copied_nodes;
};

boost::posix_time::ptime qttopt(const QDateTime &dt);

#endif // SIMULATIONSCENE_H
