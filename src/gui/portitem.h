#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>

class NodeItem;

class PortItem : public QGraphicsItem
{
public:
	PortItem(QString portName,
			 NodeItem *parent);

	virtual ~PortItem();

	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	QRectF boundingRect();

	bool isConnected() { return connected; }
	void setConnected(bool connected) { this->connected = connected; }
private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; }

	QString portName;
	bool connected;
	bool hovering;
};

#endif // PORTITEM_H
