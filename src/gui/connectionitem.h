#ifndef CONNECTIONITEM_H
#define CONNECTIONITEM_H

#include <QGraphicsItem>
#include <QPen>

class PortItem;
class NodeConnection;

class ConnectionItem : public QGraphicsItem
{
public:
	ConnectionItem(PortItem *source, QPointF second,
				   QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	ConnectionItem(PortItem *source, PortItem *sink,
				   QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	virtual ~ConnectionItem();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w = 0);
	QPainterPath shape() const {
		return connection_path.united(handle_path);
	}

	void setSink(PortItem *sink);
	void setSecond(QPointF second);
	void updatePositions();
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void setConnection(NodeConnection *con) { this->connection = con; }
	NodeConnection *getConnection() const { return connection; }
	PortItem *source, *sink;
private:
	QPainterPath connection_path, handle_path, united;
	QPointF first, second;
	NodeConnection *connection;
	bool hovered;
};

#endif // CONNECTIONITEM_H
