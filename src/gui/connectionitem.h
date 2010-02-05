#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QGraphicsPathItem>

class PortItem;
class NodeConnection;

class ConnectionItem : public QGraphicsPathItem
{
public:
	ConnectionItem(PortItem *source, QPointF second,
				   QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	ConnectionItem(PortItem *source, PortItem *sink,
				   QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	virtual ~ConnectionItem();

	void setSink(PortItem *sink);
	void setSecond(QPointF second);
	void updatePositions();
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void setConnection(NodeConnection *con) { this->connection = con; }
	NodeConnection *getConnection() const { return connection; }
	QPainterPath shape() const;
	PortItem *source, *sink;

private:
	QPointF first, second;
	NodeConnection *connection;
};

#endif // CONNECTIONITEM_H
