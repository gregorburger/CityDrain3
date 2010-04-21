#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QGraphicsItem>
#include <QPen>

class PortItem;
class NodeConnection;
class SimulationScene;

class ConnectionItem : public QObject, public QGraphicsItem
{
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	ConnectionItem(SimulationScene *scene, QString source, QString source_port_id);
	ConnectionItem(SimulationScene *scene, NodeConnection *con);

	virtual ~ConnectionItem();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w = 0);
	QPainterPath shape() const {
		return connection_path.united(handle_path);
	}

	void setSink(const QPointF &sink);
	void updatePositions();
	void updatePaths();
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void setConnection(NodeConnection *con);
	NodeConnection *getConnection() const { return connection; }

	inline const QString &getSourceId() const {
		return source_id;
	}

	inline const QString &getSinkId() const {
		return sink_id;
	}

private:
	SimulationScene *scene;
	QString source_id, source_port_id, sink_id, sink_port_id;
	QPointF source, sink;
	QPainterPath connection_path, handle_path, united;
	NodeConnection *connection;
	bool hovered;
};

#endif // CONNECTIONITEM_H
