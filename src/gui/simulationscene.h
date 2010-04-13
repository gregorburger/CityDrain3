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
	inline QList<NodeItem*> getNodeItems() const { return node_items; }

	inline QString getModelFileName() const { return model_file_name; }
	void addPlugin(QString pname);
	void addPythonModule(QString pname);

	void remove(NodeItem *item);
	void remove(ConnectionItem *item);

	//time convinience
	inline int getDt() const  { return simulation->getSimulationParameters().dt; }
	inline QDateTime getStart() const { return pttoqt(simulation->getSimulationParameters().start); }
	inline QDateTime getStop() const { return pttoqt(simulation->getSimulationParameters().stop); }

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
	QList<CopyState> copied_nodes;
	QPointF current_mouse;
};


#endif // SIMULATIONSCENE_H
