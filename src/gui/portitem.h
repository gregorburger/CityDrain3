#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QObject>

class NodeItem;
class SimulationScene;
class ConnectionItem;

class PortItem : public QObject, public QGraphicsItem {
Q_OBJECT
Q_INTERFACES(QGraphicsItem)
public:
	enum Direction { Out, In};
	PortItem(QString portName,
			 NodeItem *parent,
			 Direction dir);

	virtual ~PortItem();

	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	QRectF boundingRect() const;

	bool isConnected();

	QString getPortName() { return portName; }
	NodeItem *getNodeItem() const { return node_item; }

	void setIsAtTop() { at_top = true; }
	void setIsAtBottom() { at_bottom = true; }

private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; update(); }

	NodeItem *node_item;
	QString portName;
	Direction dir;
	bool hovering, at_top, at_bottom;
};

#endif // PORTITEM_H
