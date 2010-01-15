#ifndef SIMULATIONSCENE_H
#define SIMULATIONSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <string>

class NodeRegistry;
class MapBasedModel;
class QTreeWidget;

class SimulationScene : public QGraphicsScene
{
Q_OBJECT
public:
	SimulationScene(NodeRegistry *reg,
					QObject *parent = 0);
	virtual ~SimulationScene();
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);

private Q_SLOTS:
	void on_selectionChanged();

private:
	NodeRegistry *reg;
	QMap<std::string, int> ids;
	MapBasedModel *model;
};

#endif // SIMULATIONSCENE_H
