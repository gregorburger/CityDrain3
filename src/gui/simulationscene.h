#ifndef SIMULATIONSCENE_H
#define SIMULATIONSCENE_H

#include <string>

#include <QGraphicsScene>
#include <QMap>
#include <QDateTime>
#include <nodeitem.h> //for PortType
#include <boost/date_time/posix_time/posix_time.hpp>
#include <simulation.h>

class NodeRegistry;
class SimulationRegistry;
class MapBasedModel;
class QTreeWidget;
class PortItem;
class ConnectionItem;

boost::posix_time::ptime qttopt(const QDateTime &dt);
QDateTime pttoqt(const boost::posix_time::ptime &dt);

struct CopyState {
	std::string _class;
	QMap<std::string, std::string> parameters;
	QPointF position;
};

class SimulationScene : public QGraphicsScene
{
Q_OBJECT
	friend class DeleteConnection;
	friend class DeleteNode;
	friend class ChangeParameters;
	friend class ChangeTime;
	friend class RenameNode;
public:
	SimulationScene(QObject *parent = 0);
	virtual ~SimulationScene();
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);


	inline const NodeRegistry *getNodeRegistry() const { return node_reg; }
	inline SimulationRegistry *getSimulationRegistry() const { return sim_reg; }
	inline ISimulation *getSimulation() const { return simulation; }
	inline MapBasedModel *getModel() const { return model; }
	inline QList<NodeItem*> getNodeItems() const { return node_items.values(); }
	inline QList<ConnectionItem*> getConnectionsOf(QString node_id) { return connections_of_node.values(node_id); }

	inline QString getModelFileName() const { return model_file_name; }
	void addPlugin(QString pname);
	void addPythonModule(QString pname);

	void add(NodeItem *item);
	void add(ConnectionItem *item);
	void remove(NodeItem *item);
	void remove(ConnectionItem *item);
	bool setSimulationParameters(SimulationParameters &p);

	void renameNodeItem(QString old_id, QString new_id);

	//time convinience
	inline int getDt() const  { return simulation->getSimulationParameters().dt; }
	inline QDateTime getStart() const { return pttoqt(simulation->getSimulationParameters().start); }
	inline QDateTime getStop() const { return pttoqt(simulation->getSimulationParameters().stop); }

	NodeItem *findItem(QString id) const;
	ConnectionItem *findItem(QString source, QString source_port,
							 QString sink, QString sink_port) const;
	inline QStringList getNodeNames() const { return node_items.keys(); }
public Q_SLOTS:
	void _new();
	void save(QString path);
	void load(QString path);
	void unload();
	void copy();
	void paste();
	void deleteSelectedItems();

Q_SIGNALS:
	void changed(QUndoCommand *cmd);
	void loaded();
	void unloaded();
	void saved();
	void nodesRegistered();
	void simulationParametersChanged();

private Q_SLOTS:
	void nodeChanged(QUndoCommand *cmd);

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
	QMap<QString, NodeItem*> node_items;
	QMultiMap<QString, ConnectionItem*> connections_of_node;
	QList<ConnectionItem *> connection_items;
	QString model_file_name;
	QStringList plugins, python_modules;
	bool unsaved;
	QList<CopyState> copied_nodes;
	QPointF current_mouse;
};


#endif // SIMULATIONSCENE_H
