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

	void setSink(PortItem *sink);
	void setSecond(QPointF second);
	void updatePositions();
	void setConnection(NodeConnection *con) { this->connection = con; }
	NodeConnection *getConnection() const { return connection; }
	QPainterPath shape() const;
private:
	PortItem *source, *sink;
	QPointF first, second;
	NodeConnection *connection;
};

#endif // CONNECTIONITEM_H
