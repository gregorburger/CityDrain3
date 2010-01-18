#ifndef PORTITEM_H
#define PORTITEM_H

#include <QGraphicsItem>
#include <QDebug>
#include <QObject>

class NodeItem;
class SimulationScene;

class PortItem : public QObject, public QGraphicsItem {
Q_OBJECT
public:
	PortItem(QString portName,
			 NodeItem *parent);

	void connectConnectionSignals(SimulationScene *scene);
	virtual ~PortItem();

	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget);

	QRectF boundingRect() const;

	bool isConnected() { return connected; }
	void setConnected(bool connected) { this->connected = connected; }

	QString getPortName() { return portName; }
	NodeItem *getNodeItem() const { return node_item; }

private:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) { hovering = true; update(); }
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { hovering = false; update(); }

	NodeItem *node_item;
	QString portName;
	bool connected;
	bool hovering;
};

#endif // PORTITEM_H
